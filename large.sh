rm /home/bee/*.data
make install
curl -X POST \
   http://bee.fish/large.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T ~/bee.fish/large.json -s && \
   curl http://bee.fish/large.json > large2a.json -s && \
curl -X POST \
   http://bee.fish/large2.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large2a.json -s && \
   curl http://bee.fish/large2.json > large2b.json -s &&
diff large2a.json large2b.json