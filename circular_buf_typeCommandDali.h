#ifndef CIRCULAR_BUF_TYPE_COMMAND_DALI_H_
#define CIRCULAR_BUF_TYPE_COMMAND_DALI_H_


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

struct stDevices
{
	char strMacAddress[17];				
	char strMeasure[512];
};

// Comando Jennic
struct TypeCommand 
{
    char strCommand[256];
};

// Dali Master Command
struct TypeCommandDali
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

void init_circuler_buffer(struct circBuf_t *cb);

int circBufPush(struct circBuf_t *cb, struct TypeCommandDali data);

int circBufPop(struct circBuf_t *cb, struct TypeCommandDali *data);

#endif