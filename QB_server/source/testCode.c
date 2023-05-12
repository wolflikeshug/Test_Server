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


int testCode_c(char* path, int id, const char* content, const char* username) {
    char* fileName = calloc(10, sizeof(char));
    sprintf(fileName, "%s/%i", path, id);

    // Insert the content in the source file
    insertContentInFile(fileName, content, username, "c");

    char* executableName = calloc(50, sizeof(char));
    sprintf(executableName, "%s-%s.out", fileName, username);
    char* sourceName = calloc(50, sizeof(char));
    sprintf(sourceName, "%s-%s.c", fileName, username);
    char* compileCommand = calloc(180, sizeof(char));
    sprintf(compileCommand, "gcc -o %s %s", executableName, sourceName);

    // Compile the source file
    int compileResult = system(compileCommand);
    if (compileResult != 0) {
        printf("Compilation failed.\n");

        // Clear the files
        remove(executableName);
        remove(sourceName);
        
        // Free the memory allocated for the file names and the compile command
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);

        return 1;
    }

    // Execute the executable file
    int executeResult = system(executableName);
    if (executeResult == 0) {
        printf("Execution succeeded.\n");

        // Clear the files
        remove(executableName);
        remove(sourceName);

        // Free the memory allocated for the file names and the compile command
        free(fileName);
        free(executableName);
        free(sourceName);
        free(compileCommand);

        return 0;
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

        return 1;
    }

        
    // Clear the files
    remove(executableName);
    remove(sourceName);

    // Free the memory allocated for the file names and the compile command
    free(fileName);
    free(executableName);
    free(sourceName);
    free(compileCommand);

    return 1;
}

int testCode_py(char *path, int id, const char* content, const char* username) {

    char* fileName = calloc(10, sizeof(char));
    sprintf(fileName, "%s/%i", path, id);

    // Insert the content in the source file
    insertContentInFile(fileName, content, username, "py");

    char* executableName = calloc(50, sizeof(char));
    sprintf(executableName, "%s-%s.py", fileName, username);

    char command[256];
    sprintf(command, "python3 %s", executableName);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to execute the Python file.\n");
        remove(executableName);
        free(fileName);
        free(executableName);
        return -1;
    }

    char result[10];
    fgets(result, sizeof(result), fp);
    pclose(fp);

    result[strcspn(result, "\n")] = '\0';

    if (strcmp(result, "0") == 0) {
        printf("Execution succeeded.\n");
        remove(executableName);
        free(fileName);
        free(executableName);
        return 0;
    } else {
        printf("Execution failed.\n");
        remove(executableName);
        free(fileName);
        return 1;
    }
}
