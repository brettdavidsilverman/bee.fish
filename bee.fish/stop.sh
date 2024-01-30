#!/bin/bash
PORT=$1
if [ -z "$PORT" ]
then
      PORT=80
fi

echo "Stopping $PORT..."
PROCESS=$(sudo lsof -t -i:$PORT)
if [ -z "$PROCESS" ]
then
   echo "Nothing to do"
else
   sudo kill -9 $PROCESS
   echo "Done"
fi
