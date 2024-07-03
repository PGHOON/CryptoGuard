#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/sched.h>

#define COMMAND_LEN 16

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, char[COMMAND_LEN]);
    __type(value, __u32);
} pnames_map SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 1024);
    __type(key, __u32);
    __type(value, __u32);
} pids_map SEC(".maps");

SEC("kprobe")
int bpf_prog(struct pt_regs *ctx) {
    char pname[COMMAND_LEN] = {};
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    bpf_get_current_comm(&pname, sizeof(pname));

    __u32 *value = bpf_map_lookup_elem(&pnames_map, pname);
    if (value) {
        __u32 dummy_value = 1;
        bpf_map_update_elem(&pids_map, &pid, &dummy_value, BPF_ANY);
    }

    return 0;
}

char _license[] SEC("license") = "GPL";
