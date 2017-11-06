#include "circular_buf_typeCommandDali.h"

#define MAX_COMMAND_QUEUE2     260

struct TypeCommandDali CommandDali[MAX_COMMAND_QUEUE2];

static int is_initialized = 0;

void init_circuler_buffer(struct circBuf_t *cb) {
      cb->buffer = &CommandDali[0];
      cb->maxLen = MAX_COMMAND_QUEUE2;
      cb->tail = 0;
      cb->head = 0;
}

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

