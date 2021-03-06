#ifndef __PARSER__
#define __PARSER__

#include "models/map.h"
#include "models/airline.h"
#include <stdio.h>


Map* parseMap(const char* path);
Airline* parseAirlines(FILE* pFile, Map* map); 

#endif
