To compile:

#Prerequisites:

sudo apt install libcurl4-openssl-dev
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
tar zxvf bcm2835-1.52.tar.gz
cd bcm2835-1.52
make
sudo make install

git clone --recursive https://github.com/thedarknet/defcon25-badge.git
cd defcon25-badge/software/DarknetGateway
make
sudo ./darknet-gateway.bin
