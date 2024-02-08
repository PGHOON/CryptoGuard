#!/bin/bash

DELETE_PATH=$1

sed "/$DELETE_PATH/d" current_cron > modified_cron

crontab modified_cron

rm current_cron modified_cron

echo "Crontab containing '$DELETE_PATH' has been removed!"