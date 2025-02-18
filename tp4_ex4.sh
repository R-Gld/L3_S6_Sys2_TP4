#!/bin/bash
ps -eLf | awk 'NR!=1 {print $2}' | sort | uniq -c | awk '$1>1 {print $2}' | while read pid; do
    cmd=$(ps -p $pid -o comm=)
    thread_count=$(ps -eLf | awk -v pid=$pid '$2==pid {print $2}' | wc -l)
    echo "PID: $pid CMD: $cmd Threads: $thread_count"
done
