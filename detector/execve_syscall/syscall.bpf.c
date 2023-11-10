#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "syscall.h"

const char tp_msg[16] = "tp_execve";
struct {
    __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
} output SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, u32);
    __type(value, struct msg_t);
} my_config SEC(".maps");

struct my_syscalls_enter_execve {
	unsigned short common_type;
    unsigned char common_flags;
    unsigned char common_preempt_count;
    int common_pid;

	long syscall_nr;
	void *filename_ptr;
	long argv_ptr;
	long envp_ptr;
};

SEC("tp/syscalls/sys_enter_execve")
int tp_sys_enter_execve(struct my_syscalls_enter_execve *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), tp_msg);
   bpf_printk("%s: ctx->filename_ptr: %s", tp_msg, ctx->filename_ptr);

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));
   bpf_probe_read_user(&data.path, sizeof(data.path), ctx->filename_ptr);

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";