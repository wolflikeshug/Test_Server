#include "../header/grading.h"

// Function to read a file into a string
char *read_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, fp);
    buffer[length] = '\0';

    fclose(fp);
    return buffer;
}

int gradechoice(int id, char *filepath, char *ans) {
    char *data = read_file(filepath);
    cJSON *json = cJSON_Parse(data);
    cJSON *choices = cJSON_GetObjectItem(json, "choice");

    int size = cJSON_GetArraySize(choices);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(choices, i);
        if (cJSON_GetObjectItem(item, "id")->valueint == id) {
            char *correct_ans = cJSON_GetObjectItem(item, "answer")->valuestring;
            if (strcmp(correct_ans, ans) == 0) {
                return 0;
            } else {
                return 1;
            }
        }
    }

    return 1;
}

int grademulti(int id, char *filepath, char *ans) {
    char *data = read_file(filepath);
    cJSON *json = cJSON_Parse(data);
    cJSON *multichoices = cJSON_GetObjectItem(json, "multi-choice");

    int size = cJSON_GetArraySize(multichoices);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(multichoices, i);
        if (cJSON_GetObjectItem(item, "id")->valueint == id) {
            char *correct_ans = cJSON_GetObjectItem(item, "answer")->valuestring;
            if (strcmp(correct_ans, ans) == 0) {
                return 0;
            } else {
                return 1;
            }
        }
    }

    return 1;
}

char *choiceans(int id, char *filepath){
    char *data = read_file(filepath);
    cJSON *json = cJSON_Parse(data);
    cJSON *choices = cJSON_GetObjectItem(json, "choice");

    int size = cJSON_GetArraySize(choices);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(choices, i);
        if (cJSON_GetObjectItem(item, "id")->valueint == id) {
            char *correct_ans = cJSON_GetObjectItem(item, "answer")->valuestring;
            return correct_ans;
        }
    }
    return "error";
}

char *multians(int id,  char *filepath){
    char *data = read_file(filepath);
    cJSON *json = cJSON_Parse(data);
    cJSON *choices = cJSON_GetObjectItem(json, "multi-choice");

    int size = cJSON_GetArraySize(choices);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(choices, i);
        if (cJSON_GetObjectItem(item, "id")->valueint == id) {
            char *correct_ans = cJSON_GetObjectItem(item, "answer")->valuestring;
            return correct_ans;
        }
    }
    return "error";
}

char *codingans(int id, char *filepath){
    char *data = read_file(filepath);
    cJSON *json = cJSON_Parse(data);
    cJSON *choices = cJSON_GetObjectItem(json, "coding");

    int size = cJSON_GetArraySize(choices);
    for (int i = 0; i < size; i++) {
        cJSON *item = cJSON_GetArrayItem(choices, i);
        if (cJSON_GetObjectItem(item, "id")->valueint == id) {
            char *correct_ans = cJSON_GetObjectItem(item, "answer")->valuestring;
            return correct_ans;
        }
    }
    return "error";
}
