/*
 *   CITS3002  Project    2023-sem1
 *   Student:  23006364   HU ZHUO   100
 */
#ifndef TESTCODE_H
#define TESTCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "./config.h"

char *testCode_c(char *path, int id, const char *content, const char *username);

char *testCode_py(char *path, int id, const char *content, const char *username);

#endif // TESTCODE_H
