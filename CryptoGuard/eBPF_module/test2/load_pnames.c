#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <linux/bpf.h>
#include <string.h>
#include <unistd.h>

#define MAX_ENTRIES 1024
#define COMMAND_LEN 16

int create_bpf_map(enum bpf_map_type map_type, size_t key_size, size_t value_size, const char* map_name) {
    int map_fd;
    struct bpf_create_map_attr attr = {
        .name = map_name,
        .map_type = map_type,
        .key_size = key_size,
        .value_size = value_size,
        .max_entries = MAX_ENTRIES,
    };

    map_fd = bpf_create_map_xattr(&attr);
    if (map_fd < 0) {
        fprintf(stderr, "Error creating BPF map '%s': %s\n", map_name, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return map_fd;
}

void load_pnames_into_map(int map_fd, const char* csv_file_path) {
    FILE *file = fopen(csv_file_path, "r");
    if (!file) {
        fprintf(stderr, "Error opening CSV file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char pname[COMMAND_LEN];
        if (sscanf(line, "%15s", pname) == 1) {  // limit pname to COMMAND_LEN - 1
            __u32 value = 0;
            if (bpf_map_update_elem(map_fd, pname, &value, BPF_ANY) != 0) {
                fprintf(stderr, "Error updating BPF map: %s\n", strerror(errno));
            }
        } else {
            fprintf(stderr, "Invalid line in CSV: %s", line);
        }
    }

    fclose(file);
}

void pin_bpf_map(int map_fd, const char *pin_path) {
    if (bpf_obj_pin(map_fd, pin_path) != 0) {
        fprintf(stderr, "Error pinning BPF map: %s\n", strerror(errno));
        close(map_fd);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <csv_file_path> <pin_path1> <pin_path2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *csv_file_path = argv[1];
    const char *pin_path1 = argv[2];
    const char *pin_path2 = argv[3];

    int pnames_map_fd = create_bpf_map(BPF_MAP_TYPE_HASH, COMMAND_LEN, sizeof(__u32), "pnames_map");
    int pids_map_fd = create_bpf_map(BPF_MAP_TYPE_HASH, sizeof(__u32), sizeof(__u32), "pids_map");

    load_pnames_into_map(pnames_map_fd, csv_file_path);

    pin_bpf_map(pnames_map_fd, pin_path1);
    pin_bpf_map(pids_map_fd, pin_path2);

    printf("Process names from %s have been loaded into the BPF map and pinned at %s and %s.\n", csv_file_path, pin_path1, pin_path2);
    close(pnames_map_fd);
    close(pids_map_fd);

    return EXIT_SUCCESS;
}
