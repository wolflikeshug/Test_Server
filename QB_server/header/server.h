#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <zlib.h>

#include "./cJSON.h"
#include "./config.h"
#include "./select_questions.h"
#include "./grading.h"

#endif // SERVER_H