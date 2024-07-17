# The CPU Usage was measured using the following method.

sudo apt-get install sysstat
pidstat -u -p <PID> 1 60 > cpu_usage.log

# Benign Process

1. Nginx	|	
2. Apache	|--> apache benchmark
3. Tomcat	|
4. Redis 	 --> redis-benchmark
5. Mysql  	 --> sysbench

# Apache
ab -n 1000 -c 10 http://localhost:8080/

# Nginx
ab -n 1000 -c 10 http://localhost:8082/

# Tomcat
ab -n 1000 -c 10 http://localhost:8081/

# Redis
docker run --rm --network=host redis:latest redis-benchmark -h localhost -p 6379 -n 1000 -c 10

# Mysql
sysbench --db-driver=mysql --mysql-user=root --mysql_password=root --mysql-db=testdb --tables=10 --table-size=10000 oltp_read_write prepare

sysbench --db-driver=mysql --mysql-user=root --mysql_password=root --mysql-db=testdb --tables=10 --table-size=10000 --threads=10 --events=1000 oltp_read_write run

sysbench --db-driver=mysql --mysql-user=root --mysql_password=root --mysql-db=testdb --tables=10 --table-size=10000 oltp_read_write cleanup

