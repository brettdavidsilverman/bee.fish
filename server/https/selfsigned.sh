sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/ssl/certs/selfsigned.key -out /etc/ssl/certs/selfsigned.crt 
sudo openssl dhparam -out /etc/ssl/certs/dhparam.pem 2048


