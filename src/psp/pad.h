#ifndef	__PAD_H__
#define	__PAD_H__

#include <pspctrl.h>
#include <stdbool.h>

extern bool 		Pad_Init		(void);
extern void			Pad_Shutdown	(void);
extern void			Pad_Update  	(void);
extern bool	    	Pad_Pressed	    (const unsigned long Button);
extern bool		    Pad_Held		(const unsigned long Button);

#endif
