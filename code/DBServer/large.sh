rm /home/bee/*.data
make install
curl -X POST \
   http://bee.fish/large.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large.json -s && \
   curl http://bee.fish/large.json > large2.json -s && \
curl -X POST \
   http://bee.fish/large2.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large2.json -s && \
   curl http://bee.fish/large2.json > large2a.json -s &&
diff large2.json large2a.json