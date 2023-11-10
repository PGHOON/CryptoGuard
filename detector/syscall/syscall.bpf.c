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


//로직 매크로
//syscall_name: 모니터링 할 커널 트레이스 포인트
//tp_name: message에 출력할 메세지(tp_내용으로 통일)
#define TRACE_SYSCALL(syscall_name, tp_name) \
int trace_##syscall_name(struct kernel_tracepoints *ctx) { \
    struct data_t data = {}; \
    bpf_probe_read_kernel(&data.message, sizeof(data.message), tp_name); \
    bpf_printk(#syscall_name); \
    data.pid = bpf_get_current_pid_tgid() >> 32; \
    data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF; \
    bpf_get_current_comm(&data.command, sizeof(data.command)); \
    bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data)); \
    return 0; \
}

SEC("tp/syscalls/sys_enter_execve")
TRACE_SYSCALL(sys_enter_execve, "tp_execve")


SEC("tp/syscalls/sys_enter_accept")
int sys_enter_accept(struct kernel_tracepoints *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), "tp_accept");
   bpf_printk("sys_enter_accept");

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}

SEC("tp/syscalls/sys_enter_accept4")
int sys_enter_accept4(struct kernel_tracepoints *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), "tp_accept4");
   bpf_printk("sys_enter_accept4");

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}

SEC("tp/syscalls/sys_enter_llistxattr")
int sys_enter_llistxattr(struct kernel_tracepoints *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), "tp_llistxattr");
   bpf_printk("sys_enter_llistxattr");

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}

SEC("tp/syscalls/sys_enter_kill")
int sys_enter_kill(struct kernel_tracepoints *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), "tp_kill");
   bpf_printk("sys_enter_kill");

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}

//포함시 lima-guestagent를 지속적으로 출력
/*
SEC("tp/syscalls/sys_enter_execve")
int sys_enter_execve(struct kernel_tracepoints *ctx) {
   struct data_t data = {};

   bpf_probe_read_kernel(&data.message, sizeof(data.message), "tp_execve");
   bpf_printk("sys_enter_execve");

   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

   bpf_get_current_comm(&data.command, sizeof(data.command));

   bpf_perf_event_output(ctx, &output, BPF_F_CURRENT_CPU, &data, sizeof(data));

   return 0;
}
*/


char LICENSE[] SEC("license") = "Dual BSD/GPL";