#!/bin/bash

echo "Stopping..."
test -f server.pid && sudo pkill -F server.pid
rm -f server.pid
echo "Stopped"
