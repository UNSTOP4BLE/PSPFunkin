#include "error.h"
#include "psp/font.h"

Error error;

void ErrMSG()
{
	PrintMSG(0, 0, "%s", error.message);
}