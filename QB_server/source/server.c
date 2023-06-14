/*
 *   CITS3002  Project    2023-sem1
 *   Student:  23006364   HU ZHUO   100
 */
#include "../header/server.h"

// Define signal handler function
void signalHandler(int signum)
{
    // Handle the signal, such as reconnecting or exiting the program
    // Just do nothing about it
    printf("Caught signal %d\n", signum);
}

int main()
{
    int sock;
    struct sockaddr_in server;

    signal(SIGPIPE, signalHandler);

    char *config_file = "./config.ini";
    CONFIG cfg = {};
    read_qb_ini_file(config_file, &cfg);

    while (1)
    {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            printf("Failed to create socket\n");
            return 1;
        }

        // Set server address and port number
        server.sin_addr.s_addr = inet_addr(cfg.TM_server);
        server.sin_family = AF_INET;
        server.sin_port = htons(cfg.server_port);

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            printf("Failed to connect to server, retrying in 1 seconds\n");
            sleep(1);
            continue;
        }

        printf("Connected to server\n");

        // After successful connection, send a string to the server every 10 seconds
        while (1)
        {

            char *message_recv = (char *)malloc(262144 * sizeof(char));
            if (read(sock, message_recv, 262144) < 0)
            {
                printf("Failed to receive message\n");
                break;
            }
            else
            {
                char token = message_recv[strlen(message_recv) - 1];
                char id_s;
                int id;
                char random_s[10];
                char *message_send = (char *)malloc(262144 * sizeof(char));
                char *error_message = (char *)malloc(262144 * sizeof(char));
                char *zero = malloc(2 * sizeof(char));
                zero[0] = '0';
                message_recv[strlen(message_recv) - 1] = '\0';
                switch (token)
                {
                case 'q': // grade for choice
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    if (gradechoice(id, cfg.question_file, message_recv) == 0)
                    {
                        message_send = "0";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    else
                    {
                        message_send = "1";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    // Close the socket
                    close(sock);
                    break;

                case 'm': // grade for multiple choice
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    if (grademulti(id, cfg.question_file, message_recv) == 0)
                    {
                        message_send = "0";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    else
                    {
                        message_send = "1";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    // Close the socket
                    close(sock);
                    break;

                case 'c': // test code for c
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    sprintf(random_s, "%d", rand() % 90000 + 10000);
                    error_message = testCode_c(cfg.test_template, id, message_recv, random_s);
                    if (strcmp(error_message, zero) == 0)
                    {
                        message_send = "0";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    else
                    {
                        if (send(sock, error_message, strlen(error_message), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }

                    // Close the socket
                    close(sock);
                    break;

                case 'p': // test code for python
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    sprintf(random_s, "%d", rand() % 90000 + 10000);
                    error_message = testCode_py(cfg.test_template, id, message_recv, random_s);
                    if (strcmp(error_message, zero) == 0)
                    {
                        message_send = "0";
                        if (send(sock, message_send, strlen(message_send), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }
                    else
                    {
                        if (send(sock, error_message, strlen(error_message), 0) < 0)
                        {
                            printf("Failed to send message\n");
                        }
                    }

                    // Close the socket
                    close(sock);
                    break;

                case 'x': // return answer for single-choice question
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    message_send = choiceans(id, cfg.question_file);
                    if (send(sock, message_send, strlen(message_send), 0) < 0)
                    {
                        printf("Failed to send message\n");
                    }
                    // Close the socket
                    close(sock);
                    break;

                case 'y': // return answer for multiple-choice question
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    message_send = multians(id, cfg.question_file);
                    if (send(sock, message_send, strlen(message_send), 0) < 0)
                    {
                        printf("Failed to send message\n");
                    }
                    // Close the socket
                    close(sock);
                    break;

                case 'z': // return answer for coding question
                    id_s = message_recv[strlen(message_recv) - 1];
                    id = atoi(&id_s);
                    message_recv[strlen(message_recv) - 1] = '\0';
                    message_send = codingans(id, cfg.question_file);
                    if (send(sock, message_send, strlen(message_send), 0) < 0)
                    {
                        printf("Failed to send message\n");
                    }
                    // Close the socket
                    close(sock);
                    break;

                case 'r': // request for questions
                    QUESTION *questions = (QUESTION *)malloc(sizeof(QUESTION));
                    questions->choice_id = (int *)malloc(10 * sizeof(int));
                    questions->multi_choice_id = (int *)malloc(10 * sizeof(int));
                    questions->coding_id = (int *)malloc(10 * sizeof(int));

                    printf("%s\n", message_recv);
                    char *message_recv_part = strtok(message_recv, "|");
                    char **chosen_question = (char **)malloc(3 * sizeof(char *));
                    for (int i = 0; i < 3; i++)
                    {
                        chosen_question[i] = (char *)malloc(20 * sizeof(char));
                    }
                    int count = 0;
                    while (message_recv_part != NULL)
                    {
                        printf("%s\n", message_recv_part);
                        chosen_question[count] = message_recv_part;
                        count++;
                        message_recv_part = strtok(NULL, "|");
                    }

                    for (int i = 0; i < 3; i++)
                    {
                        char *message_recv_ppart = strtok(chosen_question[i], " ");
                        int count2 = 0;
                        while (message_recv_ppart != NULL)
                        {
                            if (i == 0)
                            {
                                questions->choice_id[count2] = atoi(message_recv_ppart);
                            }
                            else if (i == 1)
                            {
                                questions->multi_choice_id[count2] = atoi(message_recv_ppart);
                            }
                            else if (i == 2)
                            {
                                questions->coding_id[count2] = atoi(message_recv_ppart);
                            }
                            count2++;
                            message_recv_ppart = strtok(NULL, " ");
                        }
                    }

                    char *json = (char *)malloc(262144 * sizeof(char));
                    json = select_questions(cfg.question_file, questions);

                    if (send(sock, json, strlen(json), 0) < 0)
                    {
                        printf("Failed to send message\n");
                    }
                    // Close the socket
                    close(sock);
                    break;

                default:
                    break;
                }
            }
        }

        // Close the socket
        close(sock);

        // Wait for a while before attempting to reconnect
        printf("Disconnected from server, retrying in 0.5 seconds\n");
        sleep(0.5);
    }
    return 0;
}
