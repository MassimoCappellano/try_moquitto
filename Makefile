include .env

#CFLAGS=-Wall -ggdb
LDFLAGS=${MY_MOSQUITTO_LIB}/libmosquitto.so.1 -lpthread

.PHONY: all clean

all : try_mosquitto MQTTGateway

try_mosquitto : try_mosquitto.o
	${CC} $^ -o $@ ${LDFLAGS}

try_mosquitto.o : try_mosquitto.c
	${CC} -c $^ -o $@ ${CFLAGS} 

###############################################

MQTTGateway : MQTTGateway.o circular_buf_typeCommandDali.o
	${CC} $^ -o $@ ${LDFLAGS}

circular_buf_typeCommandDali.o : circular_buf_typeCommandDali.c
	${CC} -c $^ -o $@ ${CFLAGS} 

MQTTGateway.o : MQTTGateway.c
	${CC} -c $^ -o $@ ${CFLAGS} 

clean : 
	-rm -f *.o try_mosquitto MQTTGateway
