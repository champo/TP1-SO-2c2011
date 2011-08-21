#include <stdlib.h>
#include "models/theShit.h"




TheShit* initTheShit(char* name) {

    TheShit* theShit;
	if ( (theShit = malloc(sizeof(TheShit))) == NULL ) {
        return NULL;
    }
    theShit->name = name;
    theShit->id = getTheShitId(theShit->name);  //TODO
	return theShit;
}


void freeTheShit(TheShit* theShit) {
    if ( theShit != NULL) {
        free(theShit->name);
        free(theShit);
    }
}