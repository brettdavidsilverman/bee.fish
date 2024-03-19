rm /home/bee/bee.fish.data
set -e
sudo make
echo "Uploading large.json"
curl -X POST \
   https://bee.fish/large.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T /home/bee/bee.fish/large.json -s
   
echo "Downloading large.json?document"

curl https://bee.fish/large.json?document > large2.json -s

echo "Comparing large.json and large2.json"

diff /home/bee/bee.fish/large.json large2.json

echo "Downloading large2a.json"

curl https://bee.fish/large.json > large2a.json -s

echo "Uploading large2a.json"

curl -X POST \
   https://bee.fish/large2a.json \
   -H "Content-Type: application/json; charset=utf-8" \
   -H "Expect: " \
   -T large2a.json -s
   
echo "Downloading large2b.json"

curl https://bee.fish/large2a.json > large2b.json -s

echo "Comparing large2a.json and large2b.json"

diff large2a.json large2b.json

#echo "Cleaning up"

#rm -f large2a.json
#rm -f large2b.json
#rm -f large2.json