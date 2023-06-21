#!/bin/bash
PORT=$1

if test -z "$PORT" 
then
      PORT=80
fi

./stop.sh $PORT
echo "Starting..."
sudo ./build/DBServer -port $PORT &>> ~/bee.fish.log &

