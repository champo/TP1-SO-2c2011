#ifndef __THESHIT__
#define __THESHIT__

#include "utils/vector.h"

typedef struct {
    int     id;
    char*   name;
} TheShit;

int getTheShitId(char* theShitName, Vector* theShit);

void getTheShitName(int id, Vector* theShit, char* name);

TheShit* getTheShit(char* name, Vector* theShit);

TheShit* getTheShitByID(int id, Vector*theShit);

void freeTheShit(TheShit* theShit);

#endif
