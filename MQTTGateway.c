/*****************************************************************************
 *
 * MQTTDMXDaliGateway
 *
 * Daemon that runs on the OpenWRT router (or PC) and captures DMX data/command to
 * publish to MQTT broker (topic Dali).
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdio.h>			
#include <mosquitto.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Thread			
#include <pthread.h>

// Serial interface
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

// added 
#include <unistd.h>
      
        
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
// Define per definire il progetto di utilizzo

				
																
// Thread							
#define NUM_THREADS     1



#define BAUDRATE B115200

#define _POSIX_SOURCE 1 // POSIX compliant source 
#define FALSE 0
#define TRUE 1
#define DELTATIMECONFIG 0

#define MAX_COMMAND_QUEUE 50

#define NUM_MAX_VALUE         500
#define MAX_COMMAND_QUEUE2     260
#define MAXDMXTIMESTAMP 1000
#define MAXDMXDALI 500
#define MAX_CHANNELS  4
#define DELTA_VALUE	  50
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct stDevices
{
	char strMacAddress[17];				
	char strMeasure[512];
};

// Comando Jennic
typedef struct TypeCommand 
{
    char strCommand[256];
};

// Dali Master Command
typedef struct TypeCommandDali
{
      char DaliCommand[100];
      int DaliCommandLenght;
};
struct circBuf_t
{
      struct TypeCommandDali *buffer;
      int head;
      int tail;
      int maxLen;
};


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
// Thread
void *SerialThread(void *threadid);

int copychar(char *source, char *destination,int n);

int circBufPush(struct circBuf_t *cb, struct TypeCommandDali data);

int circBufPop(struct circBuf_t *cb, struct TypeCommandDali *data);

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
volatile int STOP=FALSE;
struct mosquitto *mosq = NULL; 
int nMid = 0;	
struct TypeCommandDali data;

// Default serial settings
char *MODEMDEVICE;
char *MD = "/dev/ttyUSB0";

// Topic
char* s11="/Dali/v1/AxMqttSerialDali/txDalicommand/"; //dmx -> DALI topic usato x mandare comandi al dali

time_t Time1,Time2,TimeThread1, TimeThread2, TimeGetScene, TimeGetSceneOld;
double diff=0.0;
double diffThread=0.0;
double diffTimeGetScene=0.0;

// Dichiarazione Struct
struct stDevices Device[150];
// struct dmxdalitype lastCmd;
struct TypeCommand Command[MAX_COMMAND_QUEUE]; 
struct TypeCommandDali CommandDali[MAX_COMMAND_QUEUE2];
struct circBuf_t cb;


// Dichiarazione Variabili
int StateParserMeasure = 0; 
char strSerialOut[3000];
char strCmdToCoord[256];
char strMqttMsg[1024]; // Mqtt message arrived
char strMqttTypeCmd[11];
unsigned char payload[1000];
unsigned int payloadDim;
int sdmxcmd=0; // stato macchina acquisisione seriale (0 = fine acquisizione)
int IndexCommand = 0;

// Enable/Disable topic serial (Coordinator)
bool bEnableSerial;
// Enable/Disable topic serial (Coordinator)
bool bEnableInitRead;

unsigned long timeCounterL = 0;
unsigned long timeCounterS = 0;



/****************************************************************************/
/***        Functions Definitions                                         ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: circBufPush
 *
 * DESCRIPTION: inserisce un comando nel circular buffer
 * 
 * RETURNS:
 *
 ****************************************************************************/
int circBufPush(struct circBuf_t *cb, struct TypeCommandDali data)
{
      int next = cb->head + 1;
      if (next >= cb->maxLen)
            next = 0;

      // Cicular buffer is full
      if (next == cb->tail)
            return -1;  // quit with an error

      cb->buffer[cb->head] = data;
      cb->head = next;
      return 0;
}
/****************************************************************************
 *
 * NAME: circBufPop
 *
 * DESCRIPTION: elimina un comando dal circular buffer
 * 
 * RETURNS:
 *
 ****************************************************************************/
int circBufPop(struct circBuf_t *cb, struct TypeCommandDali *data)
{
      // if the head isn't ahead of the tail, we don't have any characters
      if (cb->head == cb->tail)
            return -1;  // quit with an error

      *data = cb->buffer[cb->tail];
      //cb->buffer[cb->tail] = 0;  // clear the data (optional)

      int next = cb->tail + 1;
      if (next >= cb->maxLen)
            next = 0;

      cb->tail = next;

      return 0;
}

/****************************************************************************
 *
 * NAME: copychar
 *
 * DESCRIPTION:
 * 
 * RETURNS:		
 *
 ****************************************************************************/
int copychar(char *source, char *destination, int n)
{
	int r;
	
	for (r = 0; r < n;r++)
	{
		*(destination + r) = *(source + r);
	}
	destination[n] = 0;
	return 0;
}


/****************************************************************************/
/***        Functions Callback                                         ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: my_message_callback
 *
 * DESCRIPTION:
 * 
 * RETURNS:
 *
 ****************************************************************************/
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	
	printf("TOPIC: %s\n", message->topic);
	printf("PAYLOAD: %s\n", message->payload);
	
}


/****************************************************************************
 *
 * NAME: my_connect_callback
 *
 * DESCRIPTION:
 * 
 * RETURNS:
 *
 ****************************************************************************/
void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	int i;
	if(!result)
	{
		// Subscribe to broker information topics on successful connect (DALI command)

		//mosquitto_subscribe(mosq, NULL, s11, 2);

		// *********************************
		mosquitto_subscribe(mosq, NULL, "#", 0);

	}
	else
	{
		fprintf(stderr, "Connect failed\n");
	}
}


/****************************************************************************
 *
 * NAME: my_subscribe_callback
 *
 * DESCRIPTION:
 * 
 * RETURNS:
 *
 ****************************************************************************/
void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i;

	/*printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++)
	{
		printf(", %d", granted_qos[i]);
	}
	printf("\n");*/
}


/****************************************************************************
 *
 * NAME: my_log_callback
 *
 * DESCRIPTION:
 * 
 * RETURNS:
 *
 ****************************************************************************/
void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	// Print all log messages regardless of level
	//printf("%s\n", str);
}

/****************************************************************************
 *
 * NAME: SerialThread
 *
 * DESCRIPTION:
 * 
 * RETURNS:
 *
 ****************************************************************************/
void *SerialThread(void *threadid)
{
	int State = 0;
	volatile unsigned long timestampDMX=0; //contatore per il timestamp dei comandi
	char *pDaliCmd;
	char *pDmxCmd;

	//imposto alcuni dati di test nel vettore di strutture (poi andranno inseriti i rispettivi comandi dmx e equivalenti dali
	//....
	
	
	long tid;
	tid = (long)threadid;
	
	TimeThread1 = time(NULL);
	TimeThread2 = TimeThread1;	

	TimeGetScene = time(NULL);
	TimeGetSceneOld = TimeGetScene;

	// Serial interface
	// Initialize Serial code
	int lens = 0;
 	int fd, c, NumCharRead;
    	struct termios oldtio, newtio;
    	unsigned char strBuffer[512];
 /*       
    	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NDELAY); 
   	if (fd < 0) 
	{
		perror(MODEMDEVICE); 
		printf("Serial Port %s Error...\n\r", MODEMDEVICE);
		exit(-1); 
	}
	else
	{
		printf("Serial Port %s open correctly\r\n", MODEMDEVICE);

	}		
        
    	tcgetattr(fd, &oldtio); // Save current port settings
        
    	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	//newtio.c_iflag = IGNPAR;			
	newtio.c_oflag = 0;		
				
	// Set input mode (non-canonical, no echo,...) 
    	//newtio.c_lflag = 0;
		 
	//newtio.c_cc[VTIME]    = 0;   // inter-character timer unused 
	//newtio.c_cc[VMIN]     = 5;   // blocking read until 5 chars received 
		
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);	

*/
		
	//setup circular buffer for dali comnmand
	cb.buffer = &CommandDali[0];
      cb.maxLen = MAX_COMMAND_QUEUE2;
      cb.tail = 0;
      cb.head = 0;


	int rtdmxfound=0;
	int i = 0;
	int validate;

	
	while(1)
	{
		
		// Thread loop
		
	}			
			
	pthread_exit(NULL);
}


/****************************************************************************/
/***        Main Program 						  ***/
/****************************************************************************/
int main(int argc, char *argv[])
{
	char *id="JennetMQTTGateway";
	int i;
	char *host = "127.0.0.1"; //inserire ip gateway dali
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;
	strSerialOut[0] = '\0';
	bEnableSerial = false;
	bEnableInitRead = 0;

	
	// Get argv Parameters
	if(argc > 1)
	{
		MODEMDEVICE = argv[1];
	}
	else
	{
		MODEMDEVICE = MD;
	}
			
	

	// Thread
	pthread_t threads[NUM_THREADS];
	int rc;
	long t;
	for(t=0; t < NUM_THREADS; t++)
	{
		printf("In main: creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, SerialThread, (void *)t);
		if (rc)
		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		else
		{
			printf("Creation Thread %d completed\n", t);
		}	
	}
		
	mosquitto_lib_init();
	mosq = mosquitto_new(id, clean_session, NULL);
	if(!mosq)
	{
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	// added for username/password
	mosquitto_username_pw_set(mosq, "admin", "admin");
	// ************************************
	
	mosquitto_log_callback_set(mosq, my_log_callback);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

	if(mosquitto_connect(mosq, host, port, keepalive))
	{
		fprintf(stderr, "Unable to connect.\n");
		return 1;			
	}

	mosquitto_loop_start(mosq);
	
	struct TypeCommandDali data;

	Time1 = time(NULL);
	Time2 = Time1;
	
	while(1)
	{
		Time2 = time(NULL);
		diff = difftime(Time2, Time1);
		if(diff > DELTATIMECONFIG)
		{
			diff = 0.0;
			Time1 = time(NULL);

			// Main loop
			if (circBufPop(&cb, &data))
			{
                  //printf("CB is empty  \n\r");
            }
            else
            {
            	//printf("\nCB has an object  \n");
            	printf("\nINVIO: lenght: %d\t", data.DaliCommandLenght);
            	printf("comando: |%s|  \n", data.DaliCommand);
            	
				mosquitto_publish(mosq, &nMid, s11, data.DaliCommandLenght, data.DaliCommand, 0, false);
			}
		
		}
		// Attesa prima del prossimo ciclo
		usleep(10);
	}

	mosquitto_loop_stop(mosq, false);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}



