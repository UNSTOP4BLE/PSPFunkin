#pragma once
#include <string>      
#include <json/json.h>
#include "psp/file.h"
#include "psp/audio.h"
#include "screen.h"

extern Parser parser;
extern int notecount;

void loadChart(const char *filename);
void readInitialData();
Section readChartData(int thesection);
void tickStep(Mix_Music *song);