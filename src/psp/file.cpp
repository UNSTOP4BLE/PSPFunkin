#include "file.h"
#include <fstream>
#include "../screen.h"

void loadJson(const char *filename, Json::Value *data) 
{
	std::ifstream file(filename);
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errs;
	parseFromStream(builder, file, data, &errs);

	if (!*data)
	{
		ErrMSG( "FAILED TO FIND JSON: %s", filename);
		return;
	}

	file.close();
}

std::vector<AnimFrames> readFramesFromJson(const char *filename)
{
	//load the json file 
	Json::Value data;
	loadJson(filename, &data);

	//push frame data to the vector array
	std::vector<AnimFrames> frames(data["frames"].size());

	for (int i = 0; i < (int)data["frames"].size(); i++)
	{
		frames[i].tex = data["frames"][i][0].asInt(); //tex
		frames[i].x = data["frames"][i][1].asInt(); // x
		frames[i].y = data["frames"][i][2].asInt(); // y
		frames[i].w = data["frames"][i][3].asInt(); // w
		frames[i].h = data["frames"][i][4].asInt(); // h
		frames[i].offsetx = data["frames"][i][5].asInt(); // offset x 
		frames[i].offsety = data["frames"][i][6].asInt(); // offset y
	}
	return frames;
}

std::vector<std::vector<int>> readConfFromJson(const char *filename)
{
	//load the json file 
	Json::Value data;
	loadJson(filename, &data);

	//push config data to the vector array
	std::vector<std::vector<int>> conf(data["config"].size());

	for (int i = 0; i < (int)data["config"].size(); i++)
	{
		for (int j = 0; j < (int)data["config"][i].size(); j++)
			conf[i].push_back(data["config"][i][j].asInt());
	}
	return conf;
}