#include "characters/dad.h"
#include "psp/hash.h"

static void setChar(std::string str)
{
	//initializes a character based on their string from a songs .json file
	//you can add character specific code here
	switch (Hash::FromString(str.c_str())) {
 	    case "Char_Dad"_h:
			Dad_Init();
			game.opponent->x = 100;
			game.opponent->y = 100;
  	        break;
	}
		
}
