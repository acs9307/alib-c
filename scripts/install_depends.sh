#!/bin/bash
set -e  # Exit on error

# Install debian packages
sudo apt-get install -y doxygen cmake pkg-config

MAIN_DIR=/tmp/git
mkdir -p $MAIN_DIR

cd $MAIN_DIR

# Clean up any existing directories to avoid conflicts
rm -rf json-c zlib

# Clone repositories
echo "Cloning json-c..."
git clone https://github.com/json-c/json-c
echo "Cloning zlib..."
git clone https://github.com/madler/zlib

# Make json-c
echo "Building json-c..."
cd json-c
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install

# Make zlib
echo "Building zlib..."
cd ../../zlib
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install

# Update library cache so the linker can find the libraries
echo "Updating library cache..."
sudo ldconfig

echo "Dependencies installed successfully!"
