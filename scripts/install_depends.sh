#!/bin/bash

# Install debians.
sudo apt install doxygen

MAIN_DIR=/tmp/git
mkdir -p $MAIN_DIR 

cd $MAIN_DIR 
git clone https://github.com/json-c/json-c
git clone https://github.com/madler/zlib

# Make json-c
cd json-c
mkdir build
cd build
cmake ..
make
sudo make install

# Make zlib
cd ../../zlib
./configure
make
sudo make install
