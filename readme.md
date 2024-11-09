This builds and runs the bee.fish website

cp SampleConfig.hpp ../Config.hpp

sudo apt install build-essential

To install letsencrypt certificates

sudo certbot certonly --manual --preferred-challenges dns -d "bee.fish" -d "*.bee.fish"

sudo certbot certonly --manual --preferred-challenges dns -d "dev.bee.fish" -d "*.dev.bee.fish"

sudo chown bee /etc/letsencrypt -R

make
