CC=g++ -std=c++17 -pthread -g 
CFLAGS=\
	 -Ilibserial/api

LIBSERIAL_SRC=\
	libserial/SerialPort.cpp \
	libserial/SerialStream.cpp \
	libserial/SerialStreamBuf.cpp
LIBSERIAL_API=\
	libserial/api/*

CC1110_SRC=\
	BoardClient.cpp \
	settings.cpp \
	link_fsm.cpp \
	msg_format.cpp \
	utils.cpp \
	global.cpp \
	main.cpp
CC1110_API=\
	./*.hpp

EXE=cc1110-dev
LIBSERIAL=libserial.so.1.0
PREFIX=/usr/local/bin
LIBPREFIX=/usr/lib

.PHONY: all clean install uninstall

all: $(EXE)

$(LIBSERIAL): $(LIBSERIAL_SRC) $(LIBSERIAL_API)
	$(CC) -c -fPIC $(CFLAGS) $(LIBSERIAL_SRC)
	$(CC) -shared *.o $(CFLAGS) -o $(LIBSERIAL)

$(EXE): $(LIBSERIAL) $(CC1110_SRC) $(CC1110_API)
	$(CC) -o $(EXE) $(CFLAGS) $(CC1110_SRC) $(LIBSERIAL)

clean:
	rm *.o $(LIBSERIAL)

install:
	install $(EXE) $(PREFIX)
	install $(LIBSERIAL) $(LIBPREFIX) 
uninstall:
	rm $(PREFIX)/$(EXE)
	rm $(LIBPREFIX)/$(LIBSERIAL)
