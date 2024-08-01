#!/bin/bash

# 배열 선언 및 초기화
files=(
  "benign_wo_workload" "benign_w_workload"
)

starti=$1
endi=$2

# 배열을 순회하면서 요소 출력
for file in "${files[@]}"; do
  echo "$file"

  ./create_vms.sh $starti $endi

  sleep 60

  ./run_monitor.sh $starti $endi

  sleep 5

  if [[ "$file" == *"benign_w_workload"* ]]; then
    ./apache_workload.sh $starti $endi
  else
    sleep 30
  fi

  target_dir=dataset/${file}

  mkdir -p $target_dir

  ./pull_datasets.sh $starti $endi $target_dir

  ./clean_ssh_keys.sh $starti $endi

  ./remove_vms.sh $starti $endi

done

echo "Completed"

