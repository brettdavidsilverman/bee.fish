#!/bin/bash

echo "Stopping..."
test -f server.pid && sudo pkill -F server.pid
rm -f server.pid
#test -f tailErr.pid && sudo pkill -F tailErr.pid
#rm -f tailErr.pid
#test -f tailLog.pid && sudo pkill -F tailLog.pid
#rm -f tailLog.pid
echo "Stopped"
