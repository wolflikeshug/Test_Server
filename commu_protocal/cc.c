#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define INPUT_PIPE "/tmp/input_pipe1"
#define OUTPUT_PIPE "/tmp/output_pipe1"

void write_to_pipe(int fd, const char* data) {
    ssize_t n = write(fd, data, strlen(data));
    if (n == -1) {
        perror("Failed to write to pipe");
        exit(1);
    }
}

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

int main() {
    int input_fd = open(INPUT_PIPE, O_WRONLY);
    int output_fd = open(OUTPUT_PIPE, O_RDONLY);

    if (input_fd == -1 || output_fd == -1) {
        perror("Failed to open pipes");
        exit(1);
    }

    while(1){
        // Send commands
        write_to_pipe(input_fd, "1|127.0.0.1|8080");  // Listen on 127.0.0.1:8080
        write_to_pipe(input_fd, "3|127.0.0.1|8080|Hello, C!");  // Send message "Hello, C!" to 127.0.0.1:8080
        write_to_pipe(input_fd, "4");  // Terminate child processes

        // Read responses
        char* response = read_from_pipe(output_fd);
        printf("Response: %s\n", response);
        free(response);
    }
    close(input_fd);
    close(output_fd);

    return 0;
}
