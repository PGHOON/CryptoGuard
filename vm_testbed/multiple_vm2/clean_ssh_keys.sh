
for ((i=1; i<=50; i++))
do
    ip_suffix=$((100 + i))
    ssh-keygen -f "/home/cnslab/.ssh/known_hosts" -R "192.168.122.$ip_suffix"
done

