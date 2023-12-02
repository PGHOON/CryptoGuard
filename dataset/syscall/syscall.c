#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <bpf/libbpf.h>
#include "syscall.h"
#include "syscall.skel.h"
#include "time.h"
#include "string.h"

static FILE *csv_file;

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	if (level >= LIBBPF_DEBUG)
		return 0;

	return vfprintf(stderr, format, args);
}

void handle_event(void *ctx, int cpu, void *data, unsigned int data_sz)
{
	struct data_t *m = data;
	if(strncmp(m->command, "89bc80", 6) == 0){
	fprintf(csv_file, "%s\n", m->message);
	printf("%-6d %-6d %-16s %s\n", m->pid, m->uid, m->command, m->message);
	}
}

void lost_event(void *ctx, int cpu, long long unsigned int data_sz)
{
	printf("lost event\n");
}

int main()
{
	time_t start_time, current_time, time_stamp = 0;
	time(&start_time);

    struct syscall_bpf *skel;
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
	
	skel = syscall_bpf__open_opts(&opts);
	if (!skel) {
		printf("Failed to open BPF object\n");
		return 1;
	}

	err = syscall_bpf__load(skel);
	
	if (err) {
		printf("Failed to load BPF object\n");
		syscall_bpf__destroy(skel);
		return 1;
	}

	// Attach the progams to the events
	err = syscall_bpf__attach(skel);
	if (err) {
		fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
		syscall_bpf__destroy(skel);
        return 1;
	}

	pb = perf_buffer__new(bpf_map__fd(skel->maps.output), 8, handle_event, lost_event, NULL, NULL);
	if (!pb) {
		err = -1;
		fprintf(stderr, "Failed to create ring buffer\n");
		syscall_bpf__destroy(skel);
        return 1;
	}

	csv_file = fopen("DATASET/Malware/DATASET.csv", "w");
    if (!csv_file) {
        perror("Error opening file");
        return 1;
    }
	fprintf(csv_file, "SYSTEM_CALL\n");

	printf("[PID]  [UID]  [COMMAND]        [MESSAGE]\n");
	while (true) {
		err = perf_buffer__poll(pb, 100);
		if (err == -EINTR) {
			err = 0;
			break;
		}
		if (err < 0) {
			printf("Error polling perf buffer: %d\n", err);
			break;
		}
		time(&current_time);
		if (difftime(current_time, time_stamp) >= 5) {
			fprintf(csv_file, "TIMESTAMP\n");
			time_stamp = current_time;
		}
		if (difftime(current_time, start_time) >= 120){
			err = 0;
			break;
		}
	}

	fclose(csv_file);
	perf_buffer__free(pb);
	syscall_bpf__destroy(skel);
	return -err;
}
