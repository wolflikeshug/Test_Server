#ifndef CP_H
#define CP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <zlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>


int send_data(char *des_ip, int sockfd, char *privKey,  char *pwd);

int recv_data(int sockfd, char *privKey, char *pwd);

#endif // CP_H