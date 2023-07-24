#!/bin/sh

curl http://bee.fish/test -H "Content-Type: application/json" -d {}

curl http://bee.fish/test -H "Content-Type: text/plain" -H "Content-Length: 11" -d "Hello World" -s

curl -X POST http://bee.fish/index -H "Content-Type: text/html; charset=utf-8" -H Expect: -T sample.html -s

curl -X POST http://bee.fish/large  -H "Content-Type: application/json; charset=utf-8" -H Expect: -T large.json -s
