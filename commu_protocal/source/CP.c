#include "../header/CP.h"

// Encrypts a string using a provided public key file, signs the encrypted message using a private key with its password, and returns the encrypted and signed string
char* encrypt_and_sign_string(const char* plaintext) {
    RSA* publicRSA = NULL, * privateRSA = NULL;
    FILE* pubKeyFile = NULL, * privKeyFile = NULL;
    unsigned char* encrypted = NULL, * signedMsg = NULL;
    int encrypted_length;
    unsigned int signed_length;

    // Load public key
    pubKeyFile = fopen(publicKeyPath, "rb");
    if (!pubKeyFile) {
        fprintf(stderr, "Error opening public key file.\n");
        return NULL;
    }
    publicRSA = PEM_read_RSA_PUBKEY(pubKeyFile, NULL, NULL, NULL);
    fclose(pubKeyFile);
    if (!publicRSA) {
        fprintf(stderr, "Error loading public key.\n");
        return NULL;
    }

    // Encrypt the plaintext
    encrypted = (unsigned char*)malloc(RSA_size(publicRSA));
    encrypted_length = RSA_public_encrypt(strlen(plaintext), (unsigned char*)plaintext, encrypted, publicRSA, RSA_PKCS1_OAEP_PADDING);
    if (encrypted_length == -1) {
        fprintf(stderr, "Error encrypting the plaintext.\n");
        RSA_free(publicRSA);
        return NULL;
    }

    // Load private key
    privKeyFile = fopen(privateKeyPath, "rb");
    if (!privKeyFile) {
        fprintf(stderr, "Error opening private key file.\n");
        RSA_free(publicRSA);
        return NULL;
    }
    privateRSA = PEM_read_RSAPrivateKey(privKeyFile, NULL, NULL, NULL);
    fclose(privKeyFile);
    if (!privateRSA) {
        fprintf(stderr, "Error loading private key.\n");
        RSA_free(publicRSA);
        return NULL;
    }

    // Sign the encrypted message
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    EVP_PKEY* privKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(privKey, privateRSA);
    signedMsg = (unsigned char*)malloc(EVP_PKEY_size(privKey));
    if (!mdctx || !privKey || !signedMsg) {
        fprintf(stderr, "Error allocating memory.\n");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        return NULL;
    }
    EVP_SignInit(mdctx, EVP_sha256());
    EVP_SignUpdate(mdctx, encrypted, encrypted_length);
    EVP_SignFinal(mdctx, signedMsg, &signed_length, privKey);
    EVP_MD_CTX_free(mdctx);

    // Convert to base64 to make it easier to handle
    char* encrypted_base64 = (char*)malloc(EVP_ENCODE_LENGTH(encrypted_length));
    char* signed_base64 = (char*)malloc(EVP_ENCODE_LENGTH(signed_length));
    if (!encrypted_base64 || !signed_base64) {
        fprintf(stderr, "Error allocating memory.\n");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }
    EVP_EncodeBlock((unsigned char*)encrypted_base64, encrypted, encrypted_length);
    EVP_EncodeBlock((unsigned char*)signed_base64, signedMsg, signed_length);

    // Create the result string in "encrypted|signedMsg" format
    size_t result_length = strlen(encrypted_base64) + strlen(signed_base64) + 2;  // +2 for '|'
    char* result = (char*)malloc(result_length);
    if (!result) {
        fprintf(stderr, "Error allocating memory.\n");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        free(encrypted_base64);
        free(signed_base64);
        return NULL;
    }
    snprintf(result, result_length, "%s|%s", encrypted_base64, signed_base64);

    // Clean up
    RSA_free(publicRSA);
    RSA_free(privateRSA);
    free(encrypted);
    free(signedMsg);
    free(encrypted_base64);
    free(signed_base64);

    return result;
}

// Function to decrypt and verify the string
char* decrypt_and_verify_string(const char* encryptedAndSigned) {
    RSA *publicRSA = NULL, *privateRSA = NULL;
    FILE *pubKeyFile = NULL, *privKeyFile = NULL;
    char* decrypted = NULL;
    unsigned char* encrypted = NULL;
    unsigned char* signedMsg = NULL;
    int encrypted_length, signedMsg_length, decrypted_length;

    // Load public key
    pubKeyFile = fopen(publicKeyPath, "rb");
    if (pubKeyFile == NULL) {
        perror("Failed to open public key file");
        return NULL;
    }
    publicRSA = PEM_read_RSA_PUBKEY(pubKeyFile, NULL, NULL, NULL);
    fclose(pubKeyFile);
    if (publicRSA == NULL) {
        fprintf(stderr, "Failed to read public key");
        return NULL;
    }

    // Load private key
    privKeyFile = fopen(privateKeyPath, "rb");
    if (privKeyFile == NULL) {
        perror("Failed to open private key file");
        RSA_free(publicRSA);
        return NULL;
    }
    privateRSA = PEM_read_RSAPrivateKey(privKeyFile, NULL, NULL, NULL);
    fclose(privKeyFile);
    if (privateRSA == NULL) {
        fprintf(stderr, "Failed to read private key");
        RSA_free(publicRSA);
        return NULL;
    }

    // Decode from base64
    char* encryptedAndSigned_copy = strdup(encryptedAndSigned);  // Make a copy to avoid modifying the original string
    if (!encryptedAndSigned_copy) {
        fprintf(stderr, "Memory allocation failed");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        return NULL;
    }
    char* encrypted_str = strtok(encryptedAndSigned_copy, "|");  // Split the string at '|'
    char* signedMsg_str = strtok(NULL, "|");
    if (!encrypted_str || !signedMsg_str) {
        fprintf(stderr, "Invalid input format");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encryptedAndSigned_copy);
        return NULL;
    }

    encrypted = (unsigned char*)malloc(strlen(encrypted_str)); // Allocate memory for encrypted buffer
    signedMsg = (unsigned char*)malloc(strlen(signedMsg_str)); // Allocate memory for signed message buffer
    if (encrypted == NULL || signedMsg == NULL) {
        fprintf(stderr, "Memory allocation failed");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }

    encrypted_length = EVP_DecodeBlock(encrypted, (unsigned char*)encrypted_str, strlen(encrypted_str)) - 1;
    signedMsg_length = EVP_DecodeBlock(signedMsg, (unsigned char*)signedMsg_str, strlen(signedMsg_str)) - 1;

    // Verify the signature
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        fprintf(stderr, "Failed to create EVP_MD_CTX");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }

    EVP_PKEY* pubKey = EVP_PKEY_new();
    if (pubKey == NULL) {
        fprintf(stderr, "Failed to create EVP_PKEY");
        EVP_MD_CTX_free(mdctx);
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }

    EVP_PKEY_assign_RSA(pubKey, publicRSA);
    EVP_VerifyInit(mdctx, EVP_sha256());
    EVP_VerifyUpdate(mdctx, encrypted, encrypted_length);
    int verificationResult = EVP_VerifyFinal(mdctx, signedMsg, signedMsg_length, pubKey);
    EVP_MD_CTX_free(mdctx);
    if (verificationResult != 1) {
        fprintf(stderr, "Signature verification failure\n");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }

    // Decrypt the ciphertext
    decrypted = (char*)malloc(RSA_size(privateRSA)); // Allocate memory for decrypted buffer
    if (decrypted == NULL) {
        fprintf(stderr, "Memory allocation failed");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        return NULL;
    }

    decrypted_length = RSA_private_decrypt(encrypted_length, encrypted, (unsigned char*)decrypted, privateRSA, RSA_PKCS1_OAEP_PADDING);
    if (decrypted_length == -1) {
        fprintf(stderr, "Decryption failed\n");
        RSA_free(publicRSA);
        RSA_free(privateRSA);
        free(encrypted);
        free(signedMsg);
        free(decrypted);
        return NULL;
    }

    // Clean up
    RSA_free(publicRSA);
    RSA_free(privateRSA);
    free(encrypted);
    free(signedMsg);

    return decrypted;
}

// Calculate the CRC32 checksum of a string and return it as a string
char* calculate_crc32(const char* str) {
    // Calculate the checksum
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const Bytef*)str, strlen(str));

    // Convert the checksum to a string
    char* checksum_str = (char*)malloc(9 * sizeof(char)); // CRC32 is 32-bit, requiring 8 characters, plus the terminating '\0'
    snprintf(checksum_str, 9, "%08lx", crc);

    return checksum_str;
}


int send_data(int sockfd, const char* message) {
    ssize_t sent_bytes = send(sockfd, message, strlen(message), 0);
    if (sent_bytes == -1) {
        perror("Failed to send data");
        return 1;
    }
    return 0;
}

int recv_data(int sockfd, char* message) {
    ssize_t recv_bytes = recv(sockfd, message, BUFFER_SIZE - 1, 0);
    if (recv_bytes == -1) {
        perror("Failed to receive data");
        return 1;
    } else if (recv_bytes == 0) {
        // 远程程序关闭了连接
        return 1;
    }

    // 添加字符串结尾标志
    message[recv_bytes] = '\0';

    return 0;
}

void signal_handler(int signum) {
    // Handle the signal for sock closure
    // ...
}

int connect_to_server(int* sockfd, struct sockaddr_in* server_addr) {
    while (1) {
        // Create socket
        *sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (*sockfd == -1) {
            perror("Failed to create socket");
            return 1;
        }

        // Connect to the server
        if (connect(*sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) == -1) {
            perror("Failed to connect to server");
            close(*sockfd);
            sleep(1); // Wait for 1 second before retrying connection
        } else {
            break;
        }
    }

    return 0;
}


/*

int main() {
    char* input_buffer = malloc(BUFFER_SIZE);
    char* output_buffer = malloc(BUFFER_SIZE);
    int last_recv_result = 0; // 上一次接收消息的结果

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345); // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 设置服务器IP地址

    int sockfd;
    int input_fd, output_fd;

    mkfifo(INPUT_PIPE, 0666);
    mkfifo(OUTPUT_PIPE, 0666);

    while (1) {
        if (connect_to_server(&sockfd, &server_addr) == 0) {
            // Register the signal handler for sock closure
            signal(SIGPIPE, signal_handler);

            // Open the input and output pipes
            input_fd = open(INPUT_PIPE, O_RDONLY | O_NONBLOCK);
            output_fd = open(OUTPUT_PIPE, O_WRONLY | O_NONBLOCK);

            fd_set read_fds;
            int max_fd = (input_fd > sockfd) ? input_fd : sockfd;

            while (1) {
                FD_ZERO(&read_fds);
                FD_SET(input_fd, &read_fds);
                FD_SET(sockfd, &read_fds);

                // Set up the timeout value
                struct timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = 100000; // 100 milliseconds

                // Wait for input pipe or socket to become readable
                int ready = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

                if (ready > 0) {
                    if (FD_ISSET(input_fd, &read_fds)) {
                        // Read from input pipe
                        ssize_t read_bytes = read(input_fd, input_buffer, BUFFER_SIZE - 1);
                        if (read_bytes > 0) {
                            // Null-terminate the received data
                            input_buffer[read_bytes] = '\0';

                            // Send data to the remote program
                            send_data(sockfd, input_buffer);
                        }
                    }

                    if (FD_ISSET(sockfd, &read_fds)) {
                        // Receive data from the remote program
                        int recv_result = recv_data(sockfd, input_buffer);

                        if (recv_result == 0) {
                            // Message received successfully
                            last_recv_result = 0;
                            strcpy(output_buffer, input_buffer); // Store the received message in the output buffer

                            // Write to output pipe
                            write(output_fd, output_buffer, strlen(output_buffer));
                        } else {
                            // Message has issues, send last received correct message
                            send_data(sockfd, output_buffer);
                            last_recv_result = 1;
                        }
                    }
                }

                // Send the last received correct message
                if (last_recv_result == 0) {
                    send_data(sockfd, output_buffer);
                }

                // Clear the output buffer
                memset(output_buffer, 0, BUFFER_SIZE);

                // Simulate processing of other tasks
                sleep(1);
            }

            // Close the input and output pipes
            close(input_fd);
            close(output_fd);
        }
        
        // Close the socket
        close(sockfd);
    }

    // Free the buffer memory
    free(input_buffer);
    free(output_buffer);

    // Delete the pipe files
    unlink(INPUT_PIPE);
    unlink(OUTPUT_PIPE);

    return 0;
}


*/






int main(){
    char words[50] = "hellelo world";
    char* decrypted = calloc(50, sizeof(char));
    char* encrypted = calloc(500, sizeof(char));
    encrypted = encrypt_and_sign_string(words);
    //printf("%s\n", encrypted);
    decrypted = decrypt_and_verify_string(encrypted);
    //decrypted = "heelo world";
    printf("%s\n", decrypted);
}

