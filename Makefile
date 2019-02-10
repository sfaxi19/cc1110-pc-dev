CXX=g++-7 -std=c++17 -pthread -g 
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

.PHONY: all clean install uninstall test

all: $(LIBCC1110_DEV)

$(LIBCC1110_DEV): $(CC1110_SRC) $(CC1110_API) $(LIBSERIAL_SRC) $(LIBSERIAL_API)
	$(CXX) -c -fPIC $(CFLAGS) $(CC1110_SRC) $(LIBSERIAL_SRC)
	$(CXX) -shared *.o $(CFLAGS) -o $(LIBCC1110_DEV)

test: main.cpp
	$(CXX) main.cpp -o $(EXE) -llibcc1110-dev

clean:
	rm *.o $(LIBCC1110_DEV)

install: 
	mkdir -p $(INCLUDE_PREFIX)/cc1110-dev
	cp libcc1110-dev/api/* $(INCLUDE_PREFIX)/cc1110-dev
	cp libserial/api/* $(INCLUDE_PREFIX)/cc1110-dev
	install $(LIBCC1110_DEV) $(LIB_PREFIX)

uninstall:
	rm $(LIB_PREFIX)/$(LIBCC1110_DEV)
	rm -rf $(INCLUDE_PREFIX)/cc1110-dev
