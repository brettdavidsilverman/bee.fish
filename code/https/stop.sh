#!/bin/bash

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=443
fi

echo "Stopping " $PORT " ..."
if [[ -n "$(sudo lsof -t -i:$PORT)" ]]; then
    sudo kill "$(sudo lsof -t -i:$PORT)"
else
   echo "Not running"
fi

echo "Stopped"
