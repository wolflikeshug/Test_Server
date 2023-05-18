#ifndef CP_H
#define CP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <zlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#define INPUT_PIPE "/tmp/input_pipe"
#define OUTPUT_PIPE "/tmp/output_pipe"
#define BUFFER_SIZE (50 * 1024 * 1024) // 50MB

#define publicKeyPath "public.pem"
#define privateKeyPath "private.pem"

typedef struct sockcon {
    int sockfd;
    char* pubKey;
    char* privKey;
} SOCKCON;


//int send_data(SOCKCON sock, char* message);

//int recv_data(SOCKCON sock, char* message);

int send_data(int sockfd, const char* message);

int recv_data(int sockfd, char* message);

char* decrypt_and_verify_string(const char* encryptedAndSigned)

char* decrypt_and_verify_string(const char* encryptedAndSigned)

#endif // CP_H