/*
 *   CITS3002  Project    2023-sem1
 *   Student:  23006364   HU ZHUO   100
 */
#ifndef QB_CONFIG_H
#define QB_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

typedef struct config {
    char *test_type;
    char *question_file;
    char *test_template;
    char *TM_server;
    int TM_port;
    int server_port;
    char *RSA_priv_key;
    char *RSA_pub_key;
} CONFIG;

void read_qb_ini_file(const char *filename, CONFIG *cfg);

#endif // QB_CONFIG_H
