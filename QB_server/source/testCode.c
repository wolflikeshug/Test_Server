#include "../header/testCode.h"

void insertContentInFile(const char* fileName, const char* content, const char* username, const char* type) {
    int lineNumber = 7;

    // Open the source file
    FILE* sourceFile = fopen(fileName, "r");
    if (sourceFile == NULL) {
        printf("Error opening source file.\n");
        return;
    }

    // Create a new file with the modified content
    char newFileName[50];
    sprintf(newFileName, "%s-%s.%s", fileName, username, type);
    FILE* newFile = fopen(newFileName, "w");
    if (newFile == NULL) {
        printf("Error creating new file.\n");
        fclose(sourceFile);
        return;
    }

    // Copy content from the source file to the new file with the modification
    char line[MAX_LINE_LENGTH];
    int currentLine = 1;
    while (fgets(line, sizeof(line), sourceFile)) {
        if (currentLine == lineNumber) {
            // Insert content at the specified line
            fprintf(newFile, "%s\n", content);
        }
        fprintf(newFile, "%s", line);
        currentLine++;
    }

    // Close the source file and the new file
    fclose(sourceFile);
    fclose(newFile);
}

char *testCode_c(char *path, int id, const char *content, const char *username) {
    char *fileName = calloc(10, sizeof(char));
    sprintf(fileName, "%s/%i", path, id);

    // Insert the content in the source file
    insertContentInFile(fileName, content, username, "c");

    char *executableName = calloc(500, sizeof(char));
    sprintf(executableName, "%s-%s.out", fileName, username);
    char *sourceName = calloc(500, sizeof(char));
    sprintf(sourceName, "%s-%s.c", fileName, username);
    char *compileCommand = calloc(1800, sizeof(char));
    sprintf(compileCommand, "gcc -o %s %s 2>&1", executableName, sourceName);

    // Compile the source file
    FILE *compileOutput = popen(compileCommand, "r");
    if (compileOutput == NULL) {
        printf("Failed to execute compile command.\n");
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);
    }

    char compileResult[1256] = "";
    char buffer[1256];
    while (fgets(buffer, sizeof(buffer), compileOutput) != NULL) {
        strcat(compileResult, buffer);
    }
    int compileStatus = pclose(compileOutput);

    if (strlen(compileResult) > 0 || compileStatus != 0) {
        printf("Compilation failed.\n");

        // Clear the files
        remove(executableName);
        remove(sourceName);

        // Free the memory allocated for the file names and the compile command
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);

        return strdup(compileResult);
    }

    // Execute the executable file
    compileCommand = calloc(1800, sizeof(char));
    sprintf(compileCommand, "%s  2>&1", executableName);
    FILE *executeOutput = popen(compileCommand, "r");
    if (executeOutput == NULL) {
        printf("Failed to execute the executable.\n");
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);
    }

    char executeResult[1256] = "";
    while (fgets(buffer, sizeof(buffer), executeOutput) != NULL) {
        strcat(executeResult, buffer);
    }
    int executeStatus = pclose(executeOutput);

    if (strlen(executeResult) == 0 && executeStatus == 0) {
        printf("Execution succeeded.\n");
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);
        return strdup("0");
    } else {
        printf("Execution failed.\n");

        // Clear the files
        remove(executableName);
        remove(sourceName);

        // Free the memory allocated for the file names and the compile command
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);

        return strdup(executeResult);
    }
}


char *testCode_py(char *path, int id, const char* content, const char* username) {
    char* fileName = calloc(10, sizeof(char));
    sprintf(fileName, "%s/%i", path, id);

    // Insert the content in the source file
    insertContentInFile(fileName, content, username, "py");

    char* executableName = calloc(50, sizeof(char));
    sprintf(executableName, "%s-%s.py", fileName, username);

    char command[256];
    sprintf(command, "python3 %s 2>&1", executableName);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute the Python file.\n");
        remove(executableName);
        free(fileName);
        free(executableName);
        return strdup("Failed to execute the Python file.");
    }

    char output[1000];
    size_t output_size = fread(output, 1, sizeof(output) - 1, fp);
    output[output_size] = '\0';

    int status = pclose(fp);

    if (status == 0) {
        printf("Execution succeeded.\n");
        remove(executableName);
        free(fileName);
        free(executableName);
        return strdup("0");
    } else {
        printf("Execution failed.\n");
        char *error = strdup(output);
        remove(executableName);
        free(fileName);
        free(executableName);
        return error;
    }
}
