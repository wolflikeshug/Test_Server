/*
 *   CITS3002  Project    2023-sem1
 *   Student:  23006364   HU ZHUO   100
 */
#ifndef GRADING_H
#define GRADING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./cJSON.h"
#include "./testCode.h"

// Define JSON file path and size limits
#define FILE_PATH_SIZE 100
#define JSON_CONTENT_SIZE 5000

int gradechoice(int id, char *filepath, char *ans);

int grademulti(int id, char *filepath, char *ans);

char *choiceans(int id, char *filepath);

char *multians(int id, char *filepath);

char *codingans(int id, char *filepath);

#endif // GRADING_H