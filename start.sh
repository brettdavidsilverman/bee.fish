#!/bin/bash

./stop.sh

echo "Starting..."
./build/WebServer &
echo $! >> server.pid