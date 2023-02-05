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
Section Parser_readChartData(int thesection);
void Parser_tickStep(Mix_Music *song);