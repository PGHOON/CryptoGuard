#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <linux/bpf.h>
#include <string.h>
#include <unistd.h>

#define MAX_ENTRIES 1024

int create_bpf_map() {
    int map_fd;
    struct bpf_create_map_attr attr = {
        .name = "pids_map",
        .map_type = BPF_MAP_TYPE_HASH,
        .key_size = sizeof(__u32),
        .value_size = sizeof(__u32),
        .max_entries = MAX_ENTRIES,
    };

    map_fd = bpf_create_map_xattr(&attr);
    if (map_fd < 0) {
        fprintf(stderr, "Error creating BPF map: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return map_fd;
}

void load_pids_into_map(int map_fd, const char* csv_file_path) {
    FILE *file = fopen(csv_file_path, "r");
    if (!file) {
        fprintf(stderr, "Error opening CSV file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int pid;
        if (sscanf(line, "%d", &pid) == 1) {
            __u32 key = pid;
            __u32 value = 1;
            if (bpf_map_update_elem(map_fd, &key, &value, BPF_ANY) != 0) {
                fprintf(stderr, "Error updating BPF map: %s\n", strerror(errno));
            }
        } else {
            fprintf(stderr, "Invalid line in CSV: %s", line);
        }
    }

    fclose(file);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <csv_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *csv_file_path = argv[1];

    int map_fd = create_bpf_map();
    load_pids_into_map(map_fd, csv_file_path);

    printf("PIDs from %s have been loaded into the BPF map.\n", csv_file_path);
    close(map_fd);

    return EXIT_SUCCESS;
}

