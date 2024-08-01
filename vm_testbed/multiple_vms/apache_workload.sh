end=$((SECONDS+30))
starti=$1
endi=$2  # count 변수를 적절한 값으로 설정하세요.
while [ $SECONDS -lt $end ]; do
    for ((i=$starti; i<=$endi; i++)); do
        IP_SUFFIX=$((100 + i))
        ab -n 1000000 -c 1 http://192.168.122.${IP_SUFFIX}/ &
    done
    sleep 30
done

