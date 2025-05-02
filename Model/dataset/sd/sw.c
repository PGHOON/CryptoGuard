#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define WINDOW_SIZE 30
#define FILENAME_LEN 20

int main() {
    FILE *input_file, *output_file;
    char data[MAX_LINES][20];
    int line_count = 0;
    char filename[FILENAME_LEN];

    input_file = fopen("0001_all.txt", "r");
    if (input_file == NULL) {
        perror("open error");
        return 1;
    }

    while (fgets(data[line_count], sizeof(data[line_count]), input_file)) {
        data[line_count][strcspn(data[line_count], "\n")] = '\0';
        line_count++;
        if (line_count >= MAX_LINES) break;
    }
    fclose(input_file);

    int total_windows = line_count - WINDOW_SIZE + 1;
    for (int i = 0; i < total_windows; i++) {
        snprintf(filename, FILENAME_LEN, "%04d.txt", i + 1);

        output_file = fopen(filename, "w");
        if (output_file == NULL) {
            perror("write error");
            return 1;
        }

        for (int j = 0; j < WINDOW_SIZE; j++) {
            fprintf(output_file, "%s\n", data[i + j]);
        }
        fclose(output_file);
    }

    printf("%d\n", total_windows);
    return 0;
}