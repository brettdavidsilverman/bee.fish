#!/bin/sh

curl http://bee.fish/test -H "Content-Type: application/json" -d {}

curl http://bee.fish/test -H "Content-Type: text/plain" -H "Content-Length: 11" -d "Hello World"
# | grep "Hello World"

