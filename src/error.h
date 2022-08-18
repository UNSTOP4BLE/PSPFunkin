#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>

typedef struct 
{
	std::string message;
} Error;

extern Error error;

void ErrMSG();

#endif