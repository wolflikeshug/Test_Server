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
} CONFIG;

void read_qb_ini_file(const char *filename, CONFIG *cfg);

#endif // QB_CONFIG_H
