#include "file.h"
#include <fstream>
#include "../game.h"

void loadJson(const char *filename, Json::Value *data) 
{
    std::ifstream file(filename);
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    parseFromStream(builder, file, data, &errs);
    file.close();
    if (!data)
	{
	//	sprintf(message, "FAILED TO FIND CHART: %s", filename);
	//	game.gamestate = 4;
	//	return;
	}
}