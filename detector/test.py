import psutil
import time

interval = 1

while True:
    cpu_percent = psutil.cpu_percent(interval=interval)
    print(f"현재 CPU 사용량: {cpu_percent}%")
