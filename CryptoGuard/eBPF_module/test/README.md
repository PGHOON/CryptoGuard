# bpftool command note

gcc -o load_map load_pids.c -lbpf

sudo ./load_map pids.csv /sys/fs/bpf/pids_map

sudo bpftool map show

sudo bpftool map dump id <id>

sudo bpftool map delete id <id> key <key>

sudo rm /sys/fs/bpf/pids_map

sudo bpftool map show pinned /sys/fs/bpf/pids_map
