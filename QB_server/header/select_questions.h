#ifndef SELECT_QUESTIONS_H
#define SELECT_QUESTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./cJSON.h"

typedef struct question {
    int *choice_id;
    int *multi_choice_id;
    int *coding_id;
} QUESTION;

char* select_questions(char* json, QUESTION *q);

#endif /* SELECT_QUESTIONS_H */
