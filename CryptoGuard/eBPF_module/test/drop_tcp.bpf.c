#include <linux/bpf.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, __u32);
    __type(value, __u32);
    __uint(max_entries, 1024);
} pids_map SEC(".maps");

SEC("xdp")
int xdp_prog(struct xdp_md *ctx) {
    __u32 *pid_entry;

    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    pid_entry = bpf_map_lookup_elem(&pids_map, &pid);
    if (pid_entry) {
        return XDP_DROP;
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";