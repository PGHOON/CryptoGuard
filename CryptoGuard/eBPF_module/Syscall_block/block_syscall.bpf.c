#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, int);
    __type(value, int);
    __uint(max_entries, 1024);
} pid_map SEC(".maps");

SEC("tp/syscalls/sys_enter_execve")
int block_syscall(struct trace_event_raw_sys_enter *ctx) {
    int pid = bpf_get_current_pid_tgid() >> 32;
    int *block = bpf_map_lookup_elem(&pid_map, &pid);

    if (block) {
        return -1;  // Block the system call
    }

    return 0;  // Allow the system call
}

char LICENSE[] SEC("license") = "GPL";
