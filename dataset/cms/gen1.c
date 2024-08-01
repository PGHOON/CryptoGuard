#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>

#define HASH_SEED 1934

unsigned int hash(const char *str, int seed, int W) {
    unsigned long hash = seed;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % W;
}

typedef struct {
    int **table;
    int W;
    int D;
} CountMinSketch;

typedef struct {
    char **all_files;
    int total_files;
    int sketch_id;
    int W;
    int D;
    char *output_dir;
} ThreadData;

void initCMS(CountMinSketch *cms, int W, int D) {
    cms->W = W;
    cms->D = D;
    cms->table = (int **)malloc(D * sizeof(int *));
    for (int i = 0; i < D; i++) {
        cms->table[i] = (int *)calloc(W, sizeof(int));
    }
}

void freeCMS(CountMinSketch *cms) {
    for (int i = 0; i < cms->D; i++) {
        free(cms->table[i]);
    }
    free(cms->table);
}

void updateCMS(CountMinSketch *cms, const char *item) {
    for (int i = 0; i < cms->D; i++) {
        int index = hash(item, HASH_SEED + i, cms->W);
        cms->table[i][index]++;
    }
}

void saveCMSToCSV(CountMinSketch *cms, const char *filename) {
    char directory[256];
    strcpy(directory, filename);
    char *last_slash = strrchr(directory, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        struct stat st = {0};
        if (stat(directory, &st) == -1) {
            if (mkdir(directory, 0777) != 0 && errno != EEXIST) {
                perror("mkdir");
                return;
            }
        }
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    for (int i = 0; i < cms->D; i++) {
        for (int j = 0; j < cms->W; j++) {
            fprintf(file, "%d", cms->table[i][j]);
            if (j < cms->W - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void processCSVFiles(CountMinSketch *cms, char *files[], int num_files) {
    for (int i = 0; i < num_files; i++) {
        FILE *file = fopen(files[i], "r");
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

void collectAllFiles(const char *directory, char *all_files[], int *total_files) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    int index = 0;
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {
            all_files[index] = malloc(strlen(directory) + strlen(entry->d_name) + 2);
            if (all_files[index] == NULL) {
                perror("malloc");
                closedir(dp);
                return;
            }
            snprintf(all_files[index], 256, "%s/%s", directory, entry->d_name);
            index++;
        }
    }
    *total_files = index;

    closedir(dp);
}

void *worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    CountMinSketch cms;
    initCMS(&cms, data->W, data->D);

    processCSVFiles(&cms, data->all_files, data->total_files);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/cms_%05d.csv", data->output_dir, data->sketch_id);
    saveCMSToCSV(&cms, filename);
    freeCMS(&cms);

    return NULL;
}

void collectAndProcessFiles(const char *base_dir, int W, int D, const char *output_dir, int *sketch_id) {
    struct dirent *entry;
    DIR *dp = opendir(base_dir);
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR && strncmp(entry->d_name, "dataset_vm", 10) == 0) {
            char dir_path[256];
            snprintf(dir_path, sizeof(dir_path), "%s/%s/dataset", base_dir, entry->d_name);

            char *all_files[10000]; // Adjust as needed
            int total_files = 0;
            collectAllFiles(dir_path, all_files, &total_files);

            ThreadData thread_data;
            thread_data.all_files = all_files;
            thread_data.total_files = total_files;
            thread_data.sketch_id = (*sketch_id)++;
            thread_data.W = W;
            thread_data.D = D;
            thread_data.output_dir = (char *)output_dir;

            worker((void *)&thread_data);

            for (int i = 0; i < total_files; i++) {
                free(all_files[i]);
            }
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <W> <D>\n", argv[0]);
        return 1;
    }

    int W = atoi(argv[1]);
    int D = atoi(argv[2]);
    const char *base_dir = "./";
    const char *output_dir = "../malware_cms1";

    int sketch_id = 0;
    collectAndProcessFiles(base_dir, W, D, output_dir, &sketch_id);

    return 0;
}
