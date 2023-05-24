#!/bin/bash
PORT=$1
if [ -z "$PORT" ]
then
      PORT=80
fi

echo "Stopping $PORT..."

PID=$(sudo lsof -t -i:$PORT)
if [ -z "$PID" ]
then
   echo "Nothing to do"
else
   sudo kill $PID
   echo "Stopped process"
fi
