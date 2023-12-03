#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <bpf/libbpf.h>
#include "cryptoguard.h"
#include "cryptoguard.skel.h"
#include "time.h"
#include "string.h"
#include <limits.h>
#include <stdlib.h>

#define COMMAND_LEN 256
#define MAX_FILES 256

static FILE *csv_files[MAX_FILES];
static char csv_file_names[MAX_FILES][PATH_MAX];
static int file_count = 0;

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	if (level >= LIBBPF_DEBUG)
		return 0;

	return vfprintf(stderr, format, args);
}

void handle_event(void *ctx, int cpu, void *data, unsigned int data_sz)
{
    struct data_t *m = data;
    char file_name[COMMAND_LEN + 4 + 1];
    snprintf(file_name, sizeof(file_name), "%.256s.csv", m->command);

    FILE *csv_file = NULL;
    for (int i = 0; i < file_count; ++i) {
        if (strcmp(csv_file_names[i], file_name) == 0) {
            csv_file = csv_files[i];
            break;
        }
    }

    if (!csv_file) {
        if (file_count >= MAX_FILES) {
            fprintf(stderr, "Maximum number of files reached\n");
            return;
        }
        csv_file = fopen(file_name, "w");
        if (!csv_file) {
            fprintf(stderr, "Failed to open file: %s\n", file_name);
            return;
        }
        strcpy(csv_file_names[file_count], file_name);
        csv_files[file_count++] = csv_file;
        fprintf(csv_file, "SYSTEM_CALL\n");
    }

    fprintf(csv_file, "%s\n", m->syscall);
}

void lost_event(void *ctx, int cpu, long long unsigned int data_sz){}

int main()
{
	int init = system("rm -f buf_1/*.csv buf_2/*.csv");
	if (init != 0) {
		printf("Fail to initialize CSV files\n");
		return 1;
	}

	time_t start_time, current_time = 0;
	time(&start_time);

    struct cryptoguard_bpf *skel;
	// struct bpf_object_open_opts *o;
    int err;
	struct perf_buffer *pb = NULL;

	libbpf_set_print(libbpf_print_fn);

	char log_buf[64 * 1024];
	LIBBPF_OPTS(bpf_object_open_opts, opts,
		.kernel_log_buf = log_buf,
		.kernel_log_size = sizeof(log_buf),
		.kernel_log_level = 1,
	);
	
	skel = cryptoguard_bpf__open_opts(&opts);
	if (!skel) {
		printf("Failed to open BPF object\n");
		return 1;
	}

	err = cryptoguard_bpf__load(skel);
	
	if (err) {
		printf("Failed to load BPF object\n");
		cryptoguard_bpf__destroy(skel);
		return 1;
	}

	// Attach the progams to the events
	err = cryptoguard_bpf__attach(skel);
	if (err) {
		fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
		cryptoguard_bpf__destroy(skel);
        return 1;
	}

	pb = perf_buffer__new(bpf_map__fd(skel->maps.output), 8, handle_event, lost_event, NULL, NULL);
	if (!pb) {
		err = -1;
		fprintf(stderr, "Failed to create ring buffer\n");
		cryptoguard_bpf__destroy(skel);
        return 1;
	}

	while (true) {
		err = perf_buffer__poll(pb, 100);
		if (err == -EINTR) {
			printf("Receive SIGINT: *Detection Cancled\n");
			err = 0;
			break;
		}
		if (err < 0) {
			printf("Error polling perf buffer: %d\n", err);
			break;
		}
		time(&current_time);
		if (difftime(current_time, start_time) >= 10){
			err = 0;
			break;
		}
	}

    for (int i = 0; i < file_count; ++i) {
        if (csv_files[i]) {
            fclose(csv_files[i]);
        }
    }
	perf_buffer__free(pb);
	cryptoguard_bpf__destroy(skel);
	return -err;
}
