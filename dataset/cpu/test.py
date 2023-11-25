from bcc import BPF
from bcc.utils import printb

prog = """
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>

struct data_t {
    u32 pid;
    u64 delta;
    char comm[TASK_COMM_LEN];
};

BPF_HASH(last, u32, u64);
BPF_PERF_OUTPUT(events);

int do_count(struct pt_regs *ctx) {
    u32 pid = bpf_get_current_pid_tgid();
    u64 *last_time, delta, ts = bpf_ktime_get_ns();

    last_time = last.lookup(&pid);
    if (last_time != 0) {
        delta = ts - *last_time;
        if (delta > 1000000000) { // 1 second
            struct data_t data = {.pid = pid};
            bpf_get_current_comm(&data.comm, sizeof(data.comm));
            data.delta = delta;
            events.perf_submit(ctx, &data, sizeof(data));
        }
        last.update(&pid, &ts);
    }
    return 0;
}
"""

b = BPF(text=prog)
b.attach_kprobe(event=b.get_syscall_fnname("schedule"), fn_name="do_count")

def print_event(cpu, data, size):
    event = b["events"].event(data)
    printb(b"PID: %d COMM: %s CPU Time: %d ns" % (event.pid, event.comm, event.delta))

b["events"].open_perf_buffer(print_event)

print("Monitoring CPU usage... Hit Ctrl-C to end.")

try:
    while True:
        b.perf_buffer_poll()
except KeyboardInterrupt:
    pass

