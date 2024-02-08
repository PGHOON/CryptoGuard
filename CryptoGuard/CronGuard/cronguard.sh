#!/bin/bash

DELETE_PATH=$1

crontab -l > current_cron

sed "/$DELETE_PATH/d" current_cron > modified_cron

crontab modified_cron

rm current_cron modified_cron

echo "success"


#/home/cryptoguard/.cache/mesa_shader_cache/5d/bu31iplof

