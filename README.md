# cc1110-pc-dev
## What do you need to make it work?
* gcc-7
* two sets of CC1110dk
* two USB-UART converters
* linux PC
* install the [firmware] on the CC1110 modules
## How to compile?

```bash
cd "path to cc1110-dev"
make
sudo make install
```

## How to check?
1. First you need to compile test program:
```bash
make test
```
2. Launch receiver:
```bash
sudo ./cc1110-dev /dev/ttyUSB0 --rx
```
3. Launch transmitter:
```bash
sudo ./cc1110-dev /dev/ttyUSB1 --tx
```
### NOTICE! If you have a problems with the ch341 driver. You can [try to update] the driver on your OS.

### [How to use cc1110-dev library?]
### [How to use Smart RF studio to competent configure RF registers?]

[firmware]:https://github.com/sfaxi19/cc1110-dev
[try to update]:https://www.linux.org.ru/forum/linux-hardware/11965571?cid=13293927
[How to use cc1110-dev library?]:https://github.com/sfaxi19/cc1110-pc-dev/wiki/How-to-use-cc1110-dev
[How to use Smart RF studio to competent configure RF registers?]:https://github.com/sfaxi19/cc1110-pc-dev/wiki/How-to-use-SmartRFstudio-for-configure-RF-registers
