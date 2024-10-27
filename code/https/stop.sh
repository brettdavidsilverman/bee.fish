#!/bin/bash

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=443
fi

echo "Stopping " $PORT " ..."
if [[ -n "$(lsof -t -i:$PORT)" ]]; then
    kill "$(lsof -t -i:$PORT)"
else
   echo "Not running"
fi

echo "Stopped"
