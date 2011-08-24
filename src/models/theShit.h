#ifndef __THESHIT__
#define __THESHIT__


typedef struct {
    int     id;
    char*   name;
} TheShit;


TheShit* initTheShit(char* name);
void freeTheShit(TheShit* theShit);

#endif


