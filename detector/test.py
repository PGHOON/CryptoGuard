from bcc import BPF

# EBPF 프로그램을 로드합니다.
bpf_text = """
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>

SEC("kprobe/sys_execve")
int bpf_prog1(struct pt_regs *ctx) {
    // execve 시스템 콜의 첫 번째 인자(명령어 경로명 포인터)를 얻습니다.
    const char *filename = (const char *)PT_REGS_PARM1(ctx);

    // 파일 이름을 로그에 출력합니다.
    bpf_trace_printk("Executed command: %s\\n", filename);
    return 0;
}

char _license[] SEC("license") = "GPL";
"""

b = BPF(text=bpf_text)

# kprobe를 설정합니다.
b.attach_kprobe(event="sys_execve", fn_name="bpf_prog1")

print("Tracing sys_execve. Ctrl+C를 눌러서 종료하세요...")

# 이벤트를 무한히 대기합니다.
b.trace_print()

