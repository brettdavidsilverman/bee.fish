#!/bin/bash
PORT=$1

if test -z "$PORT" 
then
      PORT=80
fi

sudo ./stop.sh $PORT
echo "Starting..."
sudo ./build/DBServer -port $PORT -host bee.fish &
#>> ~/bee.fish.log &

