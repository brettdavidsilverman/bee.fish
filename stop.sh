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
   for item in $PID
   do
      echo $item
      sudo kill $item
   done
   echo "Stopped processes"
fi
