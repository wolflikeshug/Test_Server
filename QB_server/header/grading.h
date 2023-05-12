#ifndef GRADING_H
#define GRADING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./cJSON.h"
#include "./testCode.h"

char* grading_c(char* input, char* path, char* username, char* question_file);

char* grading_py(char* input, char* path, char* username, char* question_file);

#endif // GRADING_H