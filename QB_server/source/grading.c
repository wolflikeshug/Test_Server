#include "../header/grading.h"

// This function reads the correct answer for a question from a JSON file.
cJSON* get_correct_answer(char* path, int id, char* type) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Could not open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = malloc(length + 1);
    if (!data) {
        printf("Could not allocate memory for file data\n");
        fclose(file);
        return NULL;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON* json = cJSON_Parse(data);
    if (!json) {
        printf("Could not parse JSON data\n");
        free(data);
        return NULL;
    }

    cJSON* questions = cJSON_GetObjectItemCaseSensitive(json, type);
    if (!questions) {
        printf("Could not find questions of type: %s\n", type);
        cJSON_Delete(json);
        free(data);
        return NULL;
    }

    cJSON* item;
    cJSON* answer = NULL;

    cJSON_ArrayForEach(item, questions) {
        cJSON* idItem = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON* answerItem = cJSON_GetObjectItemCaseSensitive(item, "answer");
        if (idItem && answerItem && idItem->valueint == id) {
            answer = cJSON_Duplicate(answerItem, 1);
            break;
        }
    }

    cJSON_Delete(json);

    if (answer) {
        return answer;
    } else {
        printf("Could not find question with id: %d\n", id);
        free(data);
        return NULL;
    }
}

char* grading_c(char* input, char* path, char* username, char* question_file) {
    cJSON* json = cJSON_Parse(input);
    cJSON* result = cJSON_CreateObject();

    cJSON* choice = cJSON_GetObjectItemCaseSensitive(json, "choice");
    cJSON* multi_choice = cJSON_GetObjectItemCaseSensitive(json, "multi-choice");
    cJSON* coding = cJSON_GetObjectItemCaseSensitive(json, "coding");

    cJSON* result_choice = cJSON_CreateArray();
    cJSON* result_multi_choice = cJSON_CreateArray();
    cJSON* result_coding = cJSON_CreateArray();

    cJSON_AddItemToObject(result, "choice", result_choice);
    cJSON_AddItemToObject(result, "multi-choice", result_multi_choice);
    cJSON_AddItemToObject(result, "coding", result_coding);

    cJSON* item;
    cJSON_ArrayForEach(item, choice) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        char* answer = cJSON_GetObjectItemCaseSensitive(item, "answer")->valuestring;
        cJSON* correct_answer = get_correct_answer(question_file, id, "choice");
        if (correct_answer && strcmp(answer, correct_answer->valuestring) == 0) {
            cJSON_AddItemToArray(result_choice, cJSON_CreateNumber(id));
        }
        cJSON_Delete(correct_answer);
    }

    cJSON_ArrayForEach(item, multi_choice) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        cJSON* answers = cJSON_GetObjectItemCaseSensitive(item, "answer");
        cJSON* correct_answers = get_correct_answer(question_file, id, "multi-choice");
        int all_correct = 1;
        cJSON* answer_item;
        cJSON_ArrayForEach(answer_item, answers) {
            char* answer = answer_item->valuestring;
            int found = 0;
            cJSON* correct_answer_item;
            cJSON_ArrayForEach(correct_answer_item, correct_answers) {
                if (strcmp(answer, correct_answer_item->valuestring) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                all_correct = 0;
                break;
            }
        }
        
        cJSON* correct_answer_item;
        cJSON_ArrayForEach(correct_answer_item, correct_answers) {
            char* correct_answer = correct_answer_item->valuestring;
            int found = 0;
            cJSON* answer_item;
            cJSON_ArrayForEach(answer_item, answers) {
                char* answer = answer_item->valuestring;
                if (strcmp(answer, correct_answer) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                all_correct = 0;
                break;
            }
        }
        
        if (all_correct) {
            cJSON_AddItemToArray(result_multi_choice, cJSON_CreateNumber(id));
        }
        
        cJSON_Delete(correct_answers);
    }

    cJSON_ArrayForEach(item, coding) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        char* answer = cJSON_GetObjectItemCaseSensitive(item, "answer")->valuestring;
        if (testCode_c(path, id, answer, username) == 0) {
            cJSON_AddItemToArray(result_coding, cJSON_CreateNumber(id));
        }
    }

    char* output = cJSON_Print(result);
    cJSON_Delete(result);
    cJSON_Delete(json);

    return output;
}

char* grading_py(char* input, char* path, char* username, char* question_file) {
    cJSON* json = cJSON_Parse(input);
    cJSON* result = cJSON_CreateObject();

    cJSON* choice = cJSON_GetObjectItemCaseSensitive(json, "choice");
    cJSON* multi_choice = cJSON_GetObjectItemCaseSensitive(json, "multi-choice");
    cJSON* coding = cJSON_GetObjectItemCaseSensitive(json, "coding");

    cJSON* result_choice = cJSON_CreateArray();
    cJSON* result_multi_choice = cJSON_CreateArray();
    cJSON* result_coding = cJSON_CreateArray();

    cJSON_AddItemToObject(result, "choice", result_choice);
    cJSON_AddItemToObject(result, "multi-choice", result_multi_choice);
    cJSON_AddItemToObject(result, "coding", result_coding);

    cJSON* item;
    cJSON_ArrayForEach(item, choice) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        char* answer = cJSON_GetObjectItemCaseSensitive(item, "answer")->valuestring;
        cJSON* correct_answer = get_correct_answer(question_file, id, "choice");
        if (correct_answer && strcmp(answer, correct_answer->valuestring) == 0) {
            cJSON_AddItemToArray(result_choice, cJSON_CreateNumber(id));
        }
        cJSON_Delete(correct_answer);
    }

    cJSON_ArrayForEach(item, multi_choice) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        cJSON* answers = cJSON_GetObjectItemCaseSensitive(item, "answer");
        cJSON* correct_answers = get_correct_answer(question_file, id, "multi-choice");
        int all_correct = 1;
        cJSON* answer_item;
        cJSON_ArrayForEach(answer_item, answers) {
            char* answer = answer_item->valuestring;
            int found = 0;
            cJSON* correct_answer_item;
            cJSON_ArrayForEach(correct_answer_item, correct_answers) {
                if (strcmp(answer, correct_answer_item->valuestring) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                all_correct = 0;
                break;
            }
        }
        
        cJSON* correct_answer_item;
        cJSON_ArrayForEach(correct_answer_item, correct_answers) {
            char* correct_answer = correct_answer_item->valuestring;
            int found = 0;
            cJSON* answer_item;
            cJSON_ArrayForEach(answer_item, answers) {
                char* answer = answer_item->valuestring;
                if (strcmp(answer, correct_answer) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                all_correct = 0;
                break;
            }
        }
        
        if (all_correct) {
            cJSON_AddItemToArray(result_multi_choice, cJSON_CreateNumber(id));
        }
        
        cJSON_Delete(correct_answers);
    }

    cJSON_ArrayForEach(item, coding) {
        int id = cJSON_GetObjectItemCaseSensitive(item, "id")->valueint;
        char* answer = cJSON_GetObjectItemCaseSensitive(item, "answer")->valuestring;
        if (testCode_py(path, id, answer, username) == 0) {
            cJSON_AddItemToArray(result_coding, cJSON_CreateNumber(id));
        }
    }

    char* output = cJSON_Print(result);
    cJSON_Delete(result);
    cJSON_Delete(json);

    return output;
}
