#include "../header/server.h"

// Define signal handler function
void signalHandler(int signum) {
    // Handle the signal, such as reconnecting or exiting the program
    // Just do nothing about it
    printf("Caught signal %d\n", signum);
}



int main() {
    int sock;
    struct sockaddr_in server;
    char *message = (char *)malloc(1024 * sizeof(char));

    
    signal(SIGPIPE, signalHandler);

    char *config_file = "./config.ini";
    CONFIG cfg = {};
    read_qb_ini_file(config_file, &cfg);

    QUESTION *questions = (QUESTION *)malloc(sizeof(QUESTION));
    questions->choice_id = (int *)malloc(10 * sizeof(int));
    questions->multi_choice_id = (int *)malloc(10 * sizeof(int));
    questions->coding_id = (int *)malloc(10 * sizeof(int));

    questions->choice_id[0] = 1;
    questions->choice_id[1] = 2;

    questions->multi_choice_id[0] = 2;
    questions->multi_choice_id[1] = 4;

    questions->coding_id[0] = 3;
    questions->coding_id[1] = 5;

    char *json = (char *)malloc(1024 * sizeof(char));
    json = select_questions(cfg.question_file, questions);

    while (1) {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            printf("Failed to create socket\n");
            return 1;
        }

        // Set server address and port number
        server.sin_addr.s_addr = inet_addr(cfg.TM_server);
        server.sin_family = AF_INET;
        server.sin_port = htons(cfg.TM_port);

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            printf("Failed to connect to server, retrying in 5 seconds\n");
            sleep(5);
            continue;
        }

        printf("Connected to server\n");

        // After successful connection, send a string to the server every 10 seconds
        while (1) {

            uLong crc = crc32(0L, (const Bytef *)message, strlen(json));
            sprintf(checksum, "%lu", crc);
            strcat(checksum, json);
            if (send(sock, checksum, strlen(checksum), 0) < 0) {
                printf("Failed to send message\n");
                break;
            }

            char message_recv[1024] = "";
            if (read(sock, message_recv, 1024) < 0) {
                printf("Failed to receive message\n");
                break;
            }
            printf("Received message: %s\n", message_recv);

        }

        // Close the socket
        close(sock);

        // Wait for a while before attempting to reconnect
        printf("Disconnected from server, retrying in 5 seconds\n");
        sleep(5);
    }
    return 0;
}
