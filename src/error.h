#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>

typedef struct 
{
	char message[256];
} Error;

extern Error error;

void ErrMSG();

#endif