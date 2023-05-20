#!/bin/bash
PORT=$1
if test -z "$PORT" 
then
      PORT=80
fi
echo "Stopping..."
PID="server-$PORT.pid"
test -f $PID && sudo pkill -F $PID
rm -f $PID
echo "Stopped"
