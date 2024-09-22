sudo openssl req -x509 -nodes -days 36500 -newkey rsa:2048 -keyout selfsigned.key -out selfsigned.crt
sudo chown bee selfsigned.pem
sudo openssl dhparam -out dhparam.pem 2048
sudo chown bee selfsigned.key
sudo chown bee selfsigned.crt``


