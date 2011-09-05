#ifndef __THESHIT__
#define __THESHIT__

#include "utils/vector.h"

typedef struct {
    int     id;
    char*   name;
} TheShit;

int getTheShitId(char* theShitName, Vector* theShit);

TheShit* getTheShit(char* name, Vector* theShit);

void freeTheShit(TheShit* theShit);

#endif
