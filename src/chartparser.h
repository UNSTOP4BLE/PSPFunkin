#pragma once
#include <string>	  
#include <json/json.h>
#include "psp/file.h"
#include "psp/audio.h"
#include "screen.h"

extern Parser parser;
extern int notecount;

void Parser_loadChart(const char *filename);
void Parser_readInitialData();
void Parser_calcCrochet();
void Parser_readChartData(noteData &data);
void Parser_tickStep(Mix_Music *song);