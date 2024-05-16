#include <stdio.h>
#include <stdlib.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <errno.h>

#define PID_CSV_FILE "pids.csv"

void read_pids_and_load_to_map(int map_fd) {
    FILE *file = fopen(PID_CSV_FILE, "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int pid = atoi(line);
        int value = 1;
        if (bpf_map_update_elem(map_fd, &pid, &value, BPF_ANY) != 0) {
            perror("bpf_map_update_elem");
            exit(1);
        }
    }

    fclose(file);
}

int main(int argc, char **argv) {
    struct bpf_object *obj;
    int prog_fd, map_fd;
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.bpf.o", argv[0]);

    obj = bpf_object__open_file(filename, NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "ERROR: opening BPF object file failed\n");
        return 1;
    }

    if (bpf_object__load(obj)) {
        fprintf(stderr, "ERROR: loading BPF object file failed\n");
        return 1;
    }

    prog_fd = bpf_program__fd(bpf_object__find_program_by_title(obj, "tp/syscalls/sys_enter_execve"));
    if (prog_fd < 0) {
        fprintf(stderr, "ERROR: finding a program in the object file failed\n");
        return 1;
    }

    map_fd = bpf_map__fd(bpf_object__find_map_by_name(obj, "pid_map"));
    if (map_fd < 0) {
        fprintf(stderr, "ERROR: finding a map in the object file failed\n");
        return 1;
    }

    read_pids_and_load_to_map(map_fd);

    if (bpf_prog_attach(prog_fd, 0, BPF_TRACE_FENTRY, 0) < 0) {
    fprintf(stderr, "ERROR: attaching BPF program failed: %s\n", strerror(errno));
    return 1;
}

    printf("eBPF program loaded and attached. Press Ctrl+C to exit.\n");
    while (1) {
        sleep(1);
    }

    return 0;
}
