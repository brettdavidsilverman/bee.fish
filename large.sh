rm /home/bee/data/dev.bee.fish.data
set -e
make
make restart
DOMAIN="https://bee.fish"
echo "$DOMAIN"

echo "Uploading large.json"
curl -X POST \
   "$DOMAIN/large-test.json" \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large.json -s -k > /dev/null

echo "Downloading large-test.json?document"

curl "$DOMAIN/large-test.json?document" -s -k > large2.json

echo "Comparing large.json and large2.json"

diff large.json large2.json

echo "Downloading large2a.json"

curl "$DOMAIN/large-test.json" -s -k > large2a.json

echo "Uploading large2a.json"

curl -X POST \
   "$DOMAIN/large-test-2.json" \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large2a.json -s -k > /dev/null
   
echo "Downloading large2b.json"

curl "$DOMAIN/large-test-2.json" -s -k > large2b.json

echo "Comparing large2a.json and large2b.json"

diff large2a.json large2b.json

echo "Cleaning up"

rm -f large2a.json
rm -f large2b.json
rm -f large2.json

echo "Success 😃"
