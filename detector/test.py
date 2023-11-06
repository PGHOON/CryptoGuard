import psutil
import time

while True:
    cpu_cores = psutil.cpu_percent(interval=1, percpu=True)
    total_cpu_percent = sum(cpu_cores) / len(cpu_cores)  # 전체 CPU 사용량 계산

    for core, usage in enumerate(cpu_cores):
        print(f"CPU 코어 {core}: {usage}%")

    print(f"전체 CPU 사용량: {total_cpu_percent}%")

    # time.sleep(1)
