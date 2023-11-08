from bcc import BPF
from bcc.utils import printb

# eBPF 프로그램
# 각 CPU에서 실행 중인 태스크의 실행 시간을 모니터링합니다.
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

# BPF 프로그램 로드
b = BPF(text=prog)
b.attach_kprobe(event=b.get_syscall_fnname("schedule"), fn_name="do_count")

# 이벤트 출력 형식
def print_event(cpu, data, size):
    event = b["events"].event(data)
    printb(b"PID: %d COMM: %s CPU Time: %d ns" % (event.pid, event.comm, event.delta))

# 이벤트에 콜백 함수 연결
b["events"].open_perf_buffer(print_event)

print("Monitoring CPU usage... Hit Ctrl-C to end.")

# 무한 루프를 돌면서 이벤트 대기
try:
    while True:
        b.perf_buffer_poll()
except KeyboardInterrupt:
    pass

