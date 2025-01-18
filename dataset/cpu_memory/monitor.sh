#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <process_name> <log_file_name>"
    exit 1
fi

PROCESS_NAME="$1"
LOG_FILE="dataset/$2"

echo "Time, CPU(%), Memory(%)" > "$LOG_FILE"

for ((i=0; i<10; i+=1)); do
    ps -eo comm,%cpu,%mem | grep "$PROCESS_NAME" >> "$LOG_FILE"
    sleep 1
done

echo "Monitored $LOG_FILE."

