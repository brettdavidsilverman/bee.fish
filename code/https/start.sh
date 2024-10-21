#!/bin/bash

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=443
fi

./stop.sh $PORT

echo "Starting on port $PORT"

../../build/https -port $PORT &

#tail -f ~/server.err &
#echo $! >> tailErr.pid
#tail -f ~/server.log &
#echo $! >> tailLog.pid
