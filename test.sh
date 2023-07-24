#!/bin/sh

rm -f ~/bee.fish.data

make install

curl http://bee.fish/object -H "Content-Type: application/json" -d {}
curl http://bee.fish/object -s | grep "{}"

curl http://bee.fish/array -H "Content-Type: application/json" -d "[1,2,3,4]" -s
curl http://bee.fish/array -s | grep "[1, 2, 3, 4]"

curl -X POST http://bee.fish/index -H "Content-Type: text/html; charset=utf-8" -H Expect: -T sample.html -s
curl http://bee.fish/index -s | grep "200"

#curl -X POST http://bee.fish/large  -H "Content-Type: application/json; charset=utf-8" -H Expect: -T large.json -s
