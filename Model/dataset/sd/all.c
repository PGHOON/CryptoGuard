#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int main() {
    FILE *inputFile, *outputFile;
    char line[MAX_LINE_LENGTH];
    char *token;
    float usrValue;

    inputFile = fopen("0001.txt", "r");
    if (inputFile == NULL) {
        perror("open error");
        return 1;
    }

    outputFile = fopen("0001_all.txt", "w");
    if (outputFile == NULL) {
        perror("output error");
        fclose(inputFile);
        return 1;
    }

    while (fgets(line, sizeof(line), inputFile) != NULL) {
        if (strstr(line, "all") != NULL) {
            token = strtok(line, " ");
            token = strtok(NULL, " ");

            for (int i = 0; i < 2 && token != NULL; i++) {
                token = strtok(NULL, " ");
            }

            if (token != NULL) {
                usrValue = atof(token);
                fprintf(outputFile, "%.2f\n", usrValue);
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}