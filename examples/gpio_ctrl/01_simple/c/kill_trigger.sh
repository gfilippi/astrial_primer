kill -SIGINT `ps -aux | grep simple | grep trigger | awk '{print $2}'`
