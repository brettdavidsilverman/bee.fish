#!/bin/bash

./stop.sh

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=443
fi

echo "Starting on port $PORT"

../../build/https -port $PORT &

echo $! >> https.pid

#tail -f ~/server.err &
#echo $! >> tailErr.pid
#tail -f ~/server.log &
#echo $! >> tailLog.pid
