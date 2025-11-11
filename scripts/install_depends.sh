#!/bin/bash

set -e

# Install debians.
sudo apt update && apt-get install -y build-essential doxygen cmake git

MAIN_DIR=/tmp/git
mkdir -p $MAIN_DIR 

cd $MAIN_DIR 

# Make json-c
cd ${MAIN_DIR}
git clone https://github.com/json-c/json-c
cd ${MAIN_DIR}/json-c
mkdir -p build
cd build
cmake ..
make
sudo make install

# Make zlib
cd ${MAIN_DIR}
git clone https://github.com/madler/zlib
cd zlib
./configure
make
sudo make install
