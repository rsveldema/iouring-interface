nslookup google.com | awk '/^Address: / { print $2 }'|head -1 > /tmp/ip.txt


./out/build/Debugging/io_uring_interface  --ping `cat /tmp/ip.txt`

