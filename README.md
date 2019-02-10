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
1. Necessory to compile test program:
```bash
make test
```
2. Launch receiver:
```bash
./cc1110-dev /dev/ttyUSB0 --rx
```
3. Launch transmitter:
```bash
./cc1110-dev /dev/ttyUSB1 --tx
```

## How to use Smart RF studio to competent configure RF registers?
```
TBD
```
[firmware]:https://github.com/sfaxi19/cc1110-dev
