#ifndef __PARSER__
#define __PARSER__

#include "models/map.h"
#include "models/airline.h"
#include <stdio.h>

#define FINISHED                1
#define NAME_MAX_LENGTH         30
#define FIRST                   1

Map* parseMap(const char* path);
Airline* parseAirlines(FILE* pFile, Map* map); 

#endif
