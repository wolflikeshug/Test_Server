#include "../header/select_questions.h"

char* readFile(char *filename) {
    char *buffer = NULL;
    long length;
    FILE *f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

cJSON* remove_keys(cJSON* item, const char* key_to_remove) {
    cJSON* new_item = cJSON_Duplicate(item, 1);
    cJSON_DeleteItemFromObject(new_item, key_to_remove);
    return new_item;
}

char* select_questions(char* json_file, QUESTION *q) {
    char* text = readFile(json_file);
    cJSON *json = cJSON_Parse(text);

    cJSON *selected_questions = cJSON_CreateObject();

    cJSON *choices = cJSON_GetObjectItem(json, "choice");
    cJSON *multi_choices = cJSON_GetObjectItem(json, "multi-choice");
    cJSON *codings = cJSON_GetObjectItem(json, "coding");

    cJSON *selected_choices = cJSON_CreateArray();
    cJSON *selected_multi_choices = cJSON_CreateArray();
    cJSON *selected_codings = cJSON_CreateArray();

    for (int i = 0; q->choice_id[i] != 0; i++) {
        cJSON *item = cJSON_GetArrayItem(choices, q->choice_id[i] - 1);
        cJSON_AddItemToArray(selected_choices, remove_keys(item, "answer"));
    }

    for (int i = 0; q->multi_choice_id[i] != 0; i++) {
        cJSON *item = cJSON_GetArrayItem(multi_choices, q->multi_choice_id[i] - 1);
        cJSON_AddItemToArray(selected_multi_choices, remove_keys(item, "answer"));
    }

    for (int i = 0; q->coding_id[i] != 0; i++) {
        cJSON *item = cJSON_GetArrayItem(codings, q->coding_id[i] - 1);
        cJSON *item_without_answer = remove_keys(item, "answer");
        cJSON_DeleteItemFromObject(item_without_answer, "test_cases");
        cJSON_AddItemToArray(selected_codings, item_without_answer);
    }

    cJSON_AddItemToObject(selected_questions, "choice", selected_choices);
    cJSON_AddItemToObject(selected_questions, "multi-choice", selected_multi_choices);
    cJSON_AddItemToObject(selected_questions, "coding", selected_codings);

    char *str = cJSON_Print(selected_questions);
    
    // Cleanup
    cJSON_Delete(json);
    cJSON_Delete(selected_questions);
    free(text);

    return str;
}

