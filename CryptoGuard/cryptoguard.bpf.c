#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "syscall.h"

struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
} output SEC(".maps");

struct kernel_tracepoints {
	unsigned short common_type;
    unsigned char common_flags;
    unsigned char common_preempt_count;
    int common_pid;

	long syscall_nr;
	void *filename_ptr;
	long argv_ptr;
	long envp_ptr;
};

#define TRACE_SYSCALL(syscall_name, tp_name) \
int trace_##syscall_name(struct kernel_tracepoints *ctx) { \
    struct data_t data = {}; \
    data.pid = bpf_get_current_pid_tgid() >> 32; \
    data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF; \
    bpf_get_current_comm(&data.command, sizeof(data.command)); \
    bpf_probe_read_kernel(&data.syscall, sizeof(data.syscall), tp_name); \
    bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data)); \
    return 0; \
}

/*
Monitoring 255 syscalls
*/
SEC("tp/syscalls/sys_enter_llistxattr")
TRACE_SYSCALL(sys_enter_llistxattr, "sys_enter_llistxattr")

char LICENSE[] SEC("license") = "Dual BSD/GPL";