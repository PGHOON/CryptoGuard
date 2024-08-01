#!/bin/bash

starti=$1
endi=$2

for ((i=$starti; i<=$endi; i++))
do
    ip_suffix=$((100 + i))
    ssh-keygen -f "/home/cnslab/.ssh/known_hosts" -R "192.168.122.$ip_suffix"
done

