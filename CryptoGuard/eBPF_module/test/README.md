# bpftool command note

# userspace Program
compile: gcc -o load_map load_pids.c -lbpf

load: sudo ./load_map pids.csv /sys/fs/bpf/pids_map

useful commands:
sudo bpftool map show

sudo bpftool map dump id <id>

sudo bpftool map delete id <id> key <key>

sudo rm /sys/fs/bpf/pids_map

sudo bpftool map show pinned /sys/fs/bpf/pids_map

# eBPF Program
complile: clang -O2 -target bpf -g -D__TARGET_ARCH_x86 -c drop_tcp.bpf.c -o drop_tcp.bpf.o


load: bpftool prog load drop_tcp.bpf.o /sys/fs/bpf/drop_tcp

attatch: bpftool cgroup attach /sys/fs/cgroup/unified/ ingress pinned /sys/fs/bpf/drop_tcp
