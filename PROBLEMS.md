# PROBLEMS

https://stackoverflow.com/questions/19901934/strange-linking-error-dso-missing-from-command-line

cc MQTTGateway.o -o MQTTGateway /home/massimo/mosquitto-ssl/mosquitto-1.4.11/lib/libmosquitto.so.1
/usr/bin/ld: MQTTGateway.o: undefined reference to symbol 'pthread_create@@GLIBC_2.2.5'
//lib/x86_64-linux-gnu/libpthread.so.0: error adding symbols: DSO missing from command line
collect2: error: ld returned 1 exit status
Makefile:17: recipe for target 'MQTTGateway' failed
make: *** [MQTTGateway] Error 1

readelf -s /lib/x86_64-linux-gnu/libpthread.so.0 | grep pthread_create
   206: 0000000000007990  3720 FUNC    GLOBAL DEFAULT   14 pthread_create@@GLIBC_2.2.5
   119: 0000000000007990  3720 FUNC    LOCAL  DEFAULT   14 __pthread_create_2_1
   495: 0000000000007990  3720 FUNC    GLOBAL DEFAULT   14 pthread_create@@GLIBC_2.2
   

