import psutil
import time

while True:
    process_list = list(psutil.process_iter(attrs=['pid', 'name']))

    for process in process_list:
        process_info = process.info
        process_pid = process_info['pid']
        process_name = process_info['name']

        try:
            process = psutil.Process(process_pid)
            cpu_percent = process.cpu_percent(interval=0.01)
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            cpu_percent = 0.0

        print(f"프로세스 이름: {process_name}, PID: {process_pid}, CPU 사용량: {cpu_percent}%")

    #time.sleep(1)
