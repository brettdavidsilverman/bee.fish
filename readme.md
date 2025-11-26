This builds and runs the bee.fish website

cp SampleConfig.hpp ../Config.hpp
cp SampleMakefile ../Makefile

To install letsencrypt certificates

sudo certbot certonly --manual --preferred-challenges dns -d "bee.fish" -d "*.bee.fish"

sudo certbot certonly --manual --preferred-challenges dns -d "dev.bee.fish" -d "*.dev.bee.fish"

sudo chown <username> /etc/letsencrypt -R

sudo apt install libboost-all-dev
sudo apt install libssl-dev
sudo apt install lsof
sudo apt install build-essential

sudo make
