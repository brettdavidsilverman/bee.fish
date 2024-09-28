#!/bin/bash

echo "Stopping..."
test -f https.pid && sudo pkill -F https.pid
rm -f https.pid
#test -f tailErr.pid && sudo pkill -F tailErr.pid
#rm -f tailErr.pid
#test -f tailLog.pid && sudo pkill -F tailLog.pid
#rm -f tailLog.pid
echo "Stopped"
