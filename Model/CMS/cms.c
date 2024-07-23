#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define W 272
#define D 3
#define HASH_SEED 1934

unsigned int hash(const char *str, int seed) {
    unsigned long hash = seed;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % W;
}

typedef struct {
    int table[D][W];
} CountMinSketch;

void initCMS(CountMinSketch *cms) {
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < W; j++) {
            cms->table[i][j] = 0;
        }
    }
}

void updateCMS(CountMinSketch *cms, const char *item) {
    for (int i = 0; i < D; i++) {
        int index = hash(item, HASH_SEED + i);
        cms->table[i][index]++;
    }
}

int queryCMS(CountMinSketch *cms, const char *item) {
    int minCount = cms->table[0][hash(item, HASH_SEED)];
    for (int i = 1; i < D; i++) {
        int index = hash(item, HASH_SEED + i);
        if (cms->table[i][index] < minCount) {
            minCount = cms->table[i][index];
        }
    }
    return minCount;
}

void printCMS(CountMinSketch *cms) {
    printf("Count-Min Sketch Table:\n");
    for (int i = 0; i < D; i++) {
        for (int j = 0; j < W; j++) {
            printf("%d ", cms->table[i][j]);
        }
        printf("\n");
    }
}

void saveCMSToCSV(CountMinSketch *cms, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    for (int i = 0; i < D; i++) {
        for (int j = 0; j < W; j++) {
            fprintf(file, "%d", cms->table[i][j]);
            if (j < W - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void processCSVFiles(CountMinSketch *cms, const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
            FILE *file = fopen(filepath, "r");
            if (file == NULL) {
                perror("fopen");
                continue;
            }

            char line[1024];
            while (fgets(line, sizeof(line), file)) {
                char *system_call = strtok(line, "\n");
                if (system_call != NULL) {
                    updateCMS(cms, system_call);
                }
            }
            fclose(file);
        }
    }

    closedir(dp);
}

int main() {
    CountMinSketch cms;
    initCMS(&cms);

    processCSVFiles(&cms, "../dataset/test");

    /* QUERY TEST
    char *query_item = "sys_enter_writev";
    int count = queryCMS(&cms, query_item);
    printf("Estimated count of %s: %d\n", query_item, count);
    */

    printCMS(&cms);
    saveCMSToCSV(&cms, "cms_test.csv");

    return 0;
}
