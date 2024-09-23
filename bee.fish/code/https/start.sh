#!/bin/bash

./stop.sh

echo "Args: $1"

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=8000
fi

echo "Starting on port $PORT"

../../build/https -port $PORT &

echo $! >> ../../build/https.pid

#tail -f ~/server.err &
#echo $! >> tailErr.pid
#tail -f ~/server.log &
#echo $! >> tailLog.pid
