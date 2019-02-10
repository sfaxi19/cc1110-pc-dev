CC=g++-7 -std=c++17 -pthread -g 
CFLAGS=\
	 -Ilibserial/api \
	 -Ilibcc1110-dev/api

LIBSERIAL_SRC=\
	libserial/SerialPort.cpp \
	libserial/SerialStream.cpp \
	libserial/SerialStreamBuf.cpp
LIBSERIAL_API=\
	libserial/api/*

CC1110_SRC=\
	libcc1110-dev/src/BoardClient.cpp \
	libcc1110-dev/src/DumpParsing.cpp \
	libcc1110-dev/src/settings.cpp \
	libcc1110-dev/src/link_fsm.cpp \
	libcc1110-dev/src/msg_format.cpp \
	libcc1110-dev/src/utils.cpp \
	libcc1110-dev/src/global.cpp
CC1110_API=\
	./libcc1110-dev/api/*.hpp

EXE=cc1110-dev

LIBSERIAL=libserial.so
LIBCC1110_DEV=libcc1110-dev.so

LIB_PREFIX=/usr/lib
INCLUDE_PREFIX=/usr/include

.PHONY: clean install uninstall install-libcc1110-dev install-libserial libserial cc1110-dev test

all: test

libserial: $(LIBSERIAL_SRC) $(LIBSERIAL_API)
	$(CC) -c -fPIC $(CFLAGS) $(LIBSERIAL_SRC)
	$(CC) -shared *.o $(CFLAGS) -o $(LIBSERIAL)

cc1110-dev: $(CC1110_SRC) $(CC1110_API) $(LIBSERIAL)
	$(CC) -c -fPIC $(CFLAGS) $(CC1110_SRC)
	$(CC) -shared *.o $(CFLAGS) -o $(LIBCC1110_DEV)

test: main.cpp
	$(CC) main.cpp -o $(EXE) $(CFLAGS) -L. -lcc1110-dev -lserial

clean:
	rm *.o $(LIBSERIAL) $(LIBCC1110_DEV)

install-libcc1110-dev:
	mkdir -p $(INCLUDE_PREFIX)/cc1110-dev
	cp libcc1110-dev/api/* $(INCLUDE_PREFIX)/cc1110-dev
	install $(LIBCC1110_DEV) $(LIB_PREFIX)

install-libserial:
	mkdir -p $(INCLUDE_PREFIX)/libserial
	cp libserial/api/* $(INCLUDE_PREFIX)/libserial
	install $(LIBSERIAL) $(LIB_PREFIX)

install: install-libcc1110-dev install-libserial

uninstall:
	rm $(LIB_PREFIX)/$(LIBSERIAL)
	rm $(LIB_PREFIX)/$(LIBCC1110_DEV)
	rm -rf $(INCLUDE_PREFIX)/cc1110-dev
	rm -rf $(INCLUDE_PREFIX)/libserial
