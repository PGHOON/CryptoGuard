#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 256
#define ALLOWED_PATH "test.rules"
#define TEMP_CRON_FILE "modified_cron"

int main() {
    char current_cron_file[] = "current_cron";

    if (system("crontab -l > current_cron") == -1) {
        perror("Error executing command");
        return EXIT_FAILURE;
    }

    FILE *current_cron_fp = fopen(current_cron_file, "r");
    if (current_cron_fp == NULL) {
        perror("Error opening current_cron file");
        return EXIT_FAILURE;
    }

    FILE *modified_cron_fp = fopen(TEMP_CRON_FILE, "w");
    if (modified_cron_fp == NULL) {
        perror("Error opening modified_cron file");
        fclose(current_cron_fp);
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), current_cron_fp)) {
        if (strstr(line, ALLOWED_PATH) != NULL) {
            fputs(line, modified_cron_fp);
        } else {
            printf("Removing cron: %s", line);
        }
    }

    fclose(current_cron_fp);
    fclose(modified_cron_fp);

    if (system("crontab modified_cron") == -1) {
        perror("Error updating crontab");
        return EXIT_FAILURE;
    }

    if (unlink(current_cron_file) == -1 || unlink(TEMP_CRON_FILE) == -1) {
        perror("Error deleting temporary files");
        return EXIT_FAILURE;
    }

    printf("Crontab has been updated!\n");

    return EXIT_SUCCESS;
}
