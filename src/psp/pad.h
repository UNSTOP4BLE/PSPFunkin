#ifndef	__PAD_H__
#define	__PAD_H__

#include <pspctrl.h>
#include <stdbool.h>

typedef struct Vec2
{
	float	x, y;
} Vec2, Vec2f; 

extern bool 		Pad_Init		(void);
extern void			Pad_Shutdown	(void);
extern void			Pad_Update  	(void);
extern bool	    	Pad_Pressed	    (const unsigned long Button);
extern bool		    Pad_Held		(const unsigned long Button);
extern Vec2f*	    Pad_GetStick	(void);

#endif
