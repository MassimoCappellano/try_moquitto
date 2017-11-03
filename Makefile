CFLAGS=-Wall -ggdb
LDFLAGS=${HOME}/mosquitto-ssl/mosquitto-1.4.11/lib/libmosquitto.so.1

.PHONY: all clean

all : try_mosquitto

try_mosquitto : try_mosquitto.o
	${CC} $^ -o $@ ${LDFLAGS}

try_mosquitto.o : try_mosquitto.c
	${CC} -c $^ -o $@ ${CFLAGS} -I../../lib

clean : 
	-rm -f *.o try_mosquitto
