#! /bin/sh

sudo cp odumond /etc/init.d/odumond
sudo chmod +x /etc/init.d/odumond
sudo update-rc.d odumond defaults
