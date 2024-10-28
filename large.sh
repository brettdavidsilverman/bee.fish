rm /home/bee/data/dev.bee.fish.data
set -e
make clean debug
DOMAIN="https://test.bee.fish:8000"
echo "$DOMAIN"
curl "$DOMAIN/authenticate" -b cookies -c cookies -d "{\"method\":\"logon\",\"secret\":\"boo\"}"
echo "Uploading large.json"
curl -X POST \
   "$DOMAIN/large-test.json" \
   -b cookies -c cookies \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large.json -s -k > /dev/null

curl "$DOMAIN/authenticate" -b cookies -c cookies -d "{\"method\":\"logon\",\"secret\":\"boo\"}"

echo "Downloading large2a.json"

curl "$DOMAIN/large-test.json" -b cookies -c cookies -s -k > large2a.json

curl "$DOMAIN/authenticate" -b cookies -c cookies -d "{\"method\":\"logon\",\"secret\":\"boo\"}"

echo "Uploading large2a.json"

curl -X POST \
   "$DOMAIN/large-test-2.json" \
   -b cookies -c cookies \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large2a.json -s -k > /dev/null
   
curl "$DOMAIN/authenticate" -b cookies -c cookies -d "{\"method\":\"logon\",\"secret\":\"boo\"}"

echo "Downloading large2b.json"

curl "$DOMAIN/large-test-2.json" -b cookies -c cookies -s -k > large2b.json

echo "Comparing large2a.json and large2b.json"

diff large2a.json large2b.json

echo "Cleaning up"

rm -f large2a.json
rm -f large2b.json
rm -f large2.json

echo "Success 😃"
