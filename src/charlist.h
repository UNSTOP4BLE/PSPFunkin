#include "characters/dad.h"
#include "characters/gf.h"

#include "psp/hash.h"

static void setChar(std::string str)
{
	//initializes a character based on their string from a songs .json file
	//you can add character specific code here
	switch (Hash::FromString(str.c_str())) {
 	    case "Char_Dad"_h:
			Dad_Init();
		//	PlayState::opponent->x = 100;
		//	PlayState::opponent->y = 100;
  	        break;
 	    case "Char_GF"_h:
			GF_Init();
		//	PlayState::gf->x = 100;
		//	PlayState::gf->y = 200;
  	        break;
  	    default:
			ErrMSG("CHARACTER %s IS NULL", str.c_str());	
			return;
			break;

	}
		
}
