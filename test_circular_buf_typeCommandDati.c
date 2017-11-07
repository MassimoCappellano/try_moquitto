#include <stdio.h>

#include "circular_buf_typeCommandDali.h"


void main() {

    struct circBuf_t cb;

    init_circuler_buffer(&cb);

	struct TypeCommandDali data1 = { "PIPPO", 6};

    circBufPush(&cb, data1);
    
    struct TypeCommandDali data2 = { "PLUTO", 6};
    
    circBufPush(&cb, data2);
    
    struct TypeCommandDali dataOUT;

    if (circBufPop(&cb, &dataOUT))
    {
          printf("CB is empty  %ld\n\r", time(NULL));
    }
    else
    {
        //printf("\nCB has an object  \n");
        printf("\nINVIO: lenght: %d\t", dataOUT.DaliCommandLenght);
        printf("comando: |%s|  \n", dataOUT.DaliCommand);
        
    }

    if (circBufPop(&cb, &dataOUT))
    {
          printf("CB is empty  %ld\n\r", time(NULL));
    }
    else
    {
        //printf("\nCB has an object  \n");
        printf("\nINVIO: lenght: %d\t", dataOUT.DaliCommandLenght);
        printf("comando: |%s|  \n", dataOUT.DaliCommand);
        
    }


    if (circBufPop(&cb, &dataOUT))
    {
          printf("CB is empty  %ld\n\r", time(NULL));
    }
    else
    {
        //printf("\nCB has an object  \n");
        printf("\nINVIO: lenght: %d\t", dataOUT.DaliCommandLenght);
        printf("comando: |%s|  \n", dataOUT.DaliCommand);
        
    }


}
