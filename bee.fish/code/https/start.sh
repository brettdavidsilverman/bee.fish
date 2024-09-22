#!/bin/bash

./stop.sh

echo "Starting..."
./https -port 443 &
echo $! >> server.pid
#tail -f ~/server.err &
#echo $! >> tailErr.pid
#tail -f ~/server.log &
#echo $! >> tailLog.pid
