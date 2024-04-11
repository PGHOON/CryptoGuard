#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 256
#define MAX_LINE_LENGTH 1024

int isWhitelisted(const char *path, const char *whitelist[], int whitelist_size) {
    for (int i = 0; i < whitelist_size; ++i) {
        if (strcmp(path, whitelist[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    FILE *fp_in, *fp_out;
    char line[MAX_LINE_LENGTH];
    char delete_command[MAX_PATH_LENGTH + 30]; 

    printf("Reading crontab...\n");

    fp_in = popen("crontab -l", "r");
    if (fp_in == NULL) {
        perror("Error opening crontab");
        return EXIT_FAILURE;
    }

    int whitelist_size = 0;
    printf("Current crontab:\n");
    while (fgets(line, sizeof(line), fp_in) != NULL) {
        printf("%s", line);
        whitelist_size++;
    }

    fseek(fp_in, 0, SEEK_SET);

    const char **whitelist = malloc((whitelist_size + 1) * sizeof(char *));
    if (whitelist == NULL) {
        perror("Error allocating memory for whitelist array");
        pclose(fp_in);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < whitelist_size; ++i) {
        if (fgets(line, sizeof(line), fp_in) != NULL) {
            line[strcspn(line, "\n")] = '\0';
            whitelist[i] = strdup(line);
        } else {
            perror("Error reading crontab");
            pclose(fp_in);
            free(whitelist);
            return EXIT_FAILURE;
        }
    }
    whitelist[whitelist_size] = NULL; 

    pclose(fp_in);

    fp_out = fopen("modified_cron", "w");
    if (fp_out == NULL) {
        perror("Error creating modified_cron file");
        free(whitelist);
        return EXIT_FAILURE;
    }

    for (int i = 0; whitelist[i] != NULL; ++i) {
        if (isWhitelisted(whitelist[i], whitelist, whitelist_size)) {
            fputs(whitelist[i], fp_out);
            fputs("\n", fp_out);
        } else {
            sprintf(delete_command, "crontab -l | sed '/%s/d' | crontab", whitelist[i]);
            printf("Executing command: %s\n", delete_command);
            system(delete_command);
            printf("Crontab containing '%s' has been removed!\n", whitelist[i]);
        }
    }

    fclose(fp_out);

    for (int i = 0; i < whitelist_size; ++i) {
        free((void *)whitelist[i]);
    }
    free(whitelist);

    remove("modified_cron");

    return EXIT_SUCCESS;
}
