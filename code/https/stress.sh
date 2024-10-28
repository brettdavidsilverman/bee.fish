#!/bin/bash

curl https://bee.fish/authenticate -d "{\"method\":\"logon\",\"secret\":\"boo\"}" -b cookies -c cookies

for i in {1..200} 
do
   echo $i
   #./db.bin $i &
   curl -b cookies -c cookies https://bee.fish/num$i -d $i &
done;

echo Finished