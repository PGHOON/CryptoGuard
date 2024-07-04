#define __VMLINUX_H__

#include "vmlinux.h"
#include <linux/bpf.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>

#undef __VMLINUX_H__

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, __u32);
    __type(value, __u32);
    __uint(max_entries, 1024);
} pids_map SEC(".maps");

SEC("xdp")
int tcp_drop(struct xdp_md *ctx) {
    __u32 *target;

    __u32 pid = 1234;
    target = bpf_map_lookup_elem(&pids_map, &pid);
    if (target) {
        return XDP_DROP;
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";

//clang -target bpf -D __BPF_TRACING__ -D __TARGET_ARCH_x86 -Wall -O2 -g -o drop_tcp.o -c drop_tcp.bpf.c
//bpftool prog load drop_tcp.o /sys/fs/bpf/drop_tcp