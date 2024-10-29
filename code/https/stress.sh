#!/bin/bash
DOMAIN=https://dev.bee.fish:8000
echo $DOMAIN

curl "$DOMAIN/authenticate" -d "{\"method\":\"logon\",\"secret\":\"boo\"}" -b cookies -c cookies

for i in {1..200} 
do
   echo $i
   #./db.bin $i &
   curl -b cookies -c cookies "$DOMAIN/num$i" -d $i &
done;

echo Finished