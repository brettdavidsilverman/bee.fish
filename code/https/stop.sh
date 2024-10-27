#!/bin/bash

if [[ -n "$1" ]]; then
    PORT=$1
else
    PORT=443
fi

echo "Stopping " $PORT " ..."
for pid in $(lsof -t -i:$PORT); do
    kill $pid;
done

echo "Stopped"
