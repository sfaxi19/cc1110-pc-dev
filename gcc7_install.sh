sudo apt-get update

sudo apt-get install build-essential software-properties-common -y && \
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
sudo apt-get update

sudo apt-get install gcc-snapshot -y && \
sudo apt-get update

sudo apt-get install gcc-7 g++-7 -y
