#!/bin/bash
set -x
set pw "mypasswd"

sudo apt-get update
sudo apt-get -y install nginx

sudo apt-get -y install git
sudo apt install software-properties-common
sudo apt-key adv --recv-keys --keyserver hkp://keyserver.ubuntu.com:80 0xF1656F24C74CD1D8
sudo add-apt-repository 'deb [arch=amd64,i386,ppc64el] http://nyc2.mirrors.digitalocean.com/mariadb/repo/10.1/ubuntu xenial main'
sudo apt-get update
sudo apt-get install -y mariadb-server mariadb-client
echo "Waiting for a minute"
sleep 60
sudo systemctl enable mariadb
#sudo systemctl start mariadb

MYSQL_ROOT_PASSWORD=abcd1234


send {output=$(sudo mysql_secure_installation)}
expect {
    password: {send "$pw\r"; exp_continue}
    "#"
}

SECURE_MYSQL=$(expect -c "
	set timeout 10
	sudo spawn mysql_secure_installation
	expect \"Enter current password for root (enter for none):\"
	send \"$MYSQL\r\"
	expect \"Change the root password?\"
	send \"n\r\"
	expect \"Remove anonymous users?\"
	send \"y\r\"
	expect \"Disallow root login remotely?\"
	send \"y\r\"
	expect \"Remove test database and access to it?\"
	send \"y\r\"
	expect \"Reload privilege tables now?\"
	send \"y\r\"
	expect eof
")

echo "$SECURE_MYSQL"

aptitude -y purge expect
  
  