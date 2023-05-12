#ifndef TESTCODE_H
#define TESTCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "./config.h"

int testCode_c(char *path, int id, const char* content, const char* username);

int testCode_py(char *path, int id, const char* content, const char* username);

#endif // TESTCODE_H
