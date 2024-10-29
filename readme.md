To install letsencrypt certificates

sudo certbot certonly --manual --preferred-challenges dns -d "bee.fish"

sudo chown bee /etc/letsencrypt -R
