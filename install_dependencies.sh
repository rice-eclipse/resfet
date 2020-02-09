#!/bin/bash
wget http://www.open.com.au/mikem/bcm2835/bcm2835-1.62.tar.gz
tar xvfz bcm2835-1.62.tar.gz
cd bcm2835-1.62
./configure
make
sudo make install
cd ..
rm bcm2835-1.62.tar.gz
rm -rf bcm2835-1.62
