#!/bin/bash
PORT=$1

if test -z "$PORT" 
then
      PORT=80
fi

./stop.sh $PORT
PID="server-$PORT.pid"
echo "Starting..."
sudo ./build/DBServer -port $PORT &
echo $! > $PID
