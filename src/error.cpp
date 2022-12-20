#include "error.h"
#include "psp/font.h"

char message[256];
void ErrMSG()
{
	while(1)
	{
		PrintMSG(0, 0, "ERROR");
		PrintMSG(0, 20, message);
	}
}