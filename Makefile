#CFLAGS=-Wall -ggdb
LDFLAGS=${HOME}/mosquitto-ssl/mosquitto-1.4.11/lib/libmosquitto.so.1 -lpthread

.PHONY: all clean

all : try_mosquitto MQTTGateway

try_mosquitto : try_mosquitto.o
	${CC} $^ -o $@ ${LDFLAGS}

try_mosquitto.o : try_mosquitto.c
	${CC} -c $^ -o $@ ${CFLAGS} 

###############################################

MQTTGateway : MQTTGateway.o
	${CC} $^ -o $@ ${LDFLAGS}

MQTTGateway.o : MQTTGateway.c
	${CC} -c $^ -o $@ ${CFLAGS} 

clean : 
	-rm -f *.o try_mosquitto MQTTGateway
