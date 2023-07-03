#!/bin/sh

curl http://bee.fish/test -H "Content-Type: application/json" -d {}

curl http://bee.fish/test -H "Content-Type: text/plain" -H "Content-Length: 11" -d "Hello World" -s

time curl -X POST -H "Content-Type: application/json; charset=utf-8" -H Expect: -T large.json -s http://bee.fish/test >/dev/null
