#pragma once
#include <string>      
#include "psp/audio.h"

extern Parser parser;
extern int notecount;

void loadChart(const char *filename);
void readInitialData();
Section readChartData(int thesection);
void tickStep(Mix_Music *song);