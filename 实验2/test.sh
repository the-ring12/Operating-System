#!/bin/bash

for i in {1..5}
do
    echo "--------$i------" >> test.txt
    ./test2 >> test.txt &
    PID=$!
    echo $PID
    sleep 6
    kill -SIGINT $PID

    sleep 16
done
