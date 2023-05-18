#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define INPUT_PIPE "/tmp/input_pipe"
#define OUTPUT_PIPE "/tmp/output_pipe"

// Signal handler
void signal_handler(int signum) {
    // Do nothing, just to prevent termination
}

// Function to create pipe
int create_pipe(const char* path) {
    mkfifo(path, 0666);
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Failed to open pipe");
        exit(1);
    }
    return fd;
}

// Function to read from pipe
char* read_from_pipe(int fd) {
    char* buffer = malloc(1024);
    if (!buffer) {
        perror("Failed to allocate memory");
        exit(1);
    }
    ssize_t n = read(fd, buffer, 1024);
    if (n == -1) {
        perror("Failed to read from pipe");
        free(buffer);
        exit(1);
    }
    buffer[n] = '\0';
    return buffer;
}

// Function to write to pipe
void write_to_pipe(int fd, const char* data) {
    ssize_t n = write(fd, data, strlen(data));
    if (n == -1) {
        perror("Failed to write to pipe");
        exit(1);
    }
}

// Function to create socket
int create_socket(const char* ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        perror("Failed to set server address");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Failed to connect to server");
        exit(1);
    }

    return sockfd;
}

// Function to close socket
void close_socket(int sockfd) {
    close(sockfd);
}

typedef struct {
    pid_t pid;
    char* ip;
    int port;
} ChildProcess;

ChildProcess* children = NULL;
size_t num_children = 0;

// Function to create child process
ChildProcess create_child(const char* ip, int port) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(1);
    }

    if (pid > 0) {  // Parent process
        children = realloc(children, (num_children + 1) * sizeof(ChildProcess));
        if (!children) {
            perror("Failed to allocate memory");
            exit(1);
        }
        children[num_children].pid = pid;
        children[num_children].ip = strdup(ip);
        children[num_children].port = port;
        num_children++;
    }

    return (ChildProcess){pid, (char*)ip, port};
}

// Function to destroy child process
void destroy_child(ChildProcess* child) {
    kill(child->pid, SIGTERM);
    free(child->ip);
    for (size_t i = 0; i < num_children; i++) {
        if (children[i].pid == child->pid) {
            memmove(children + i, children + i + 1, (num_children - i - 1) * sizeof(ChildProcess));
            num_children--;
            children = realloc(children, num_children * sizeof(ChildProcess));
            if (!children && num_children) {
                perror("Failed to allocate memory");
                exit(1);
            }
            break;
        }
    }
}

// Function to destroy child processes
void destroy_all_children() {
    for (size_t i = 0; i < num_children; i++) {
        kill(children[i].pid, SIGTERM);
        free(children[i].ip);
    }
    free(children);
    children = NULL;
    num_children = 0;
}

// Function to find child process
ChildProcess* find_child(const char* ip, int port) {
    for (size_t i = 0; i < num_children; i++) {
        if (strcmp(children[i].ip, ip) == 0 && children[i].port == port) {
            return &children[i];
        }
    }
    return NULL;
}

// Function to handle command
void handle_command(int input_fd, int output_fd, const char* cmd) {
    char* data = read_from_pipe(input_fd);
    char* ip;
    int port;
    switch (cmd[0]) {
        case '1':  // Listen
            sscanf(data, "%ms|%d", &ip, &port);
            ChildProcess child = create_child(ip, port);
            if (child.pid == 0) {  // Child process
                int sockfd = create_socket(ip, port);
                while (1) {
                    char buffer[1024];
                    ssize_t n = read(sockfd, buffer, sizeof(buffer));
                    if (n <= 0) {
                        write(sockfd, "01", 2);  // Resend request
                    } else {
                        buffer[n] = '\0';
                        write_to_pipe(output_fd, buffer);
                    }
                }
            }
            break;
        case '2':  // Close
            sscanf(data, "%ms|%d", &ip, &port);
            ChildProcess* child_to_destroy = find_child(ip, port);
            if (child_to_destroy) {
                destroy_child(child_to_destroy);
            }
            break;
        case '3':  // Send
            char* message;
            sscanf(data, "%ms|%d|%ms", &ip, &port, &message);
            pid_t pid_send = fork();
            if (pid_send == 0) {  // Child process
                int sockfd = create_socket(ip, port);
                write(sockfd, message, strlen(message));
                close_socket(sockfd);
                exit(0);
            }
            break;
        case '4':  // Terminate
            destroy_all_children();
            exit(0);
            break;
    }
    free(data);
}

int main() {
    signal(SIGPIPE, signal_handler);

    int input_fd = create_pipe(INPUT_PIPE);
    int output_fd = create_pipe(OUTPUT_PIPE);

    fd_set set;
    FD_ZERO(&set);
    FD_SET(input_fd, &set);

    while (1) {
        fd_set read_set = set;
        if (select(input_fd + 1, &read_set, NULL, NULL, NULL) == -1) {
            perror("Failed to select");
            exit(1);
        }
        if (FD_ISSET(input_fd, &read_set)) {
            char* cmd = read_from_pipe(input_fd);
            handle_command(input_fd, output_fd, cmd);
            free(cmd);
        }
    }

    return 0;
}
