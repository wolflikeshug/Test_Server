#include "../header/config.h"

// Function to get the value of a field from qb.ini file
void read_qb_ini_file(const char *filename, CONFIG *cfg)
{
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    char *key, *value;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Ignore comment lines
        if (line[0] == '#')
            continue;

        key = strtok(line, "=");
        if (key == NULL)
            continue;

        // Remove leading and trailing whitespace
        while (isspace(*key))
            key++;
        char *end = key + strlen(key) - 1;
        while (isspace(*end))
            end--;
        *(end + 1) = '\0';

        // Check if this is the field we are looking for
        if (strcmp(key, "test_type") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->test_type = strdup(value);
        }
        else if (strcmp(key, "question_file") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->question_file = strdup(value);
        }
        else if (strcmp(key, "test_template") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->test_template = strdup(value);
        }
        else if (strcmp(key, "TM_server") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->TM_server = strdup(value);
        }
        else if (strcmp(key, "TM_port") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->TM_port = atoi(value);
        }
        else if (strcmp(key, "server_port") == 0)
        {
            value = strtok(NULL, "=");
            if (value == NULL)
                break;

            // Remove leading and trailing whitespace
            while (isspace(*value))
                value++;
            end = value + strlen(value) - 1;
            while (isspace(*end))
                end--;
            *(end + 1) = '\0';

            cfg->server_port = atoi(value);
        }
    }
    fclose(fp);
}
