#! /bin/sh

sudo update-rc.d -f odumond remove
sudo rm /etc/init.d/odumond
sudo rm /var/run/odumond.pid
sudo rm /var/log/odumond.log
