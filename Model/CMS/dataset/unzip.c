#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define DEST_DIR "../new_0/"
#define TEMP_DIR "./temp_dir/"

void extract_tar_gz(const char *filename);
void rename_and_move_csv(const char *directory, int *file_counter);

int main() {
    const char *tar_files[] = {
        "csv_files_001.tar.gz", "csv_files_002.tar.gz", "csv_files_003.tar.gz",
        "csv_files_004.tar.gz", "csv_files_005.tar.gz", "csv_files_006.tar.gz",
        "csv_files_007.tar.gz", "csv_files_008.tar.gz", "csv_files_009.tar.gz",
        "csv_files_010.tar.gz", "csv_files_011.tar.gz", "csv_files_012.tar.gz",
        "csv_files_013.tar.gz", "csv_files_014.tar.gz", "csv_files_015.tar.gz",
        "csv_files_016.tar.gz", "csv_files_017.tar.gz", "csv_files_018.tar.gz",
        "csv_files_019.tar.gz", "csv_files_020.tar.gz", "csv_files_021.tar.gz",
        "csv_files_022.tar.gz", "csv_files_023.tar.gz", "csv_files_024.tar.gz",
        "csv_files_025.tar.gz", "csv_files_026.tar.gz", "csv_files_027.tar.gz",
        "csv_files_028.tar.gz", "csv_files_029.tar.gz", "csv_files_030.tar.gz",
        "csv_files_031.tar.gz", "csv_files_032.tar.gz", "csv_files_033.tar.gz",
        "csv_files_034.tar.gz", "csv_files_035.tar.gz", "csv_files_036.tar.gz",
        "csv_files_037.tar.gz", "csv_files_038.tar.gz", "csv_files_039.tar.gz",
        "csv_files_040.tar.gz", "csv_files_041.tar.gz", "csv_files_042.tar.gz",
        "csv_files_043.tar.gz", "csv_files_044.tar.gz", "csv_files_045.tar.gz",
        "csv_files_046.tar.gz", "csv_files_047.tar.gz", "csv_files_048.tar.gz",
        "csv_files_049.tar.gz", "csv_files_050.tar.gz"
    };
    int num_files = sizeof(tar_files) / sizeof(tar_files[0]);
    int file_counter = 1;

    for (int i = 0; i < num_files; ++i) {
        printf("Processing %s\n", tar_files[i]);
        extract_tar_gz(tar_files[i]);
        rename_and_move_csv(TEMP_DIR, &file_counter);

        char cleanup_command[256];
        snprintf(cleanup_command, sizeof(cleanup_command), "rm -rf %s*", TEMP_DIR);
        system(cleanup_command);
    }

    return 0;
}

void extract_tar_gz(const char *filename) {
    char command[256];
    snprintf(command, sizeof(command), "mkdir -p %s && tar -xzf %s -C %s", TEMP_DIR, filename, TEMP_DIR);
    system(command);
}

void rename_and_move_csv(const char *directory, int *file_counter) {
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".csv")) {
                char old_path[256];
                char new_path[256];
                snprintf(old_path, sizeof(old_path), "%s%s", directory, dir->d_name);
                snprintf(new_path, sizeof(new_path), "%snew_%04d.csv", DEST_DIR, (*file_counter)++);

                printf("Renaming and moving %s to %s\n", old_path, new_path);
                rename(old_path, new_path);
            }
        }
        closedir(d);
    }
}


