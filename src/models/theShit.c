#include <stdlib.h>
#include "models/theShit.h"



//TODO THIS IS WRONG! JUST HARD-CODING! THIS SHOULD RETURN A REFERENCE TO AN
//ALREADY CREATED STRUCTURE. 
TheShit* getTheShit(char* name) {

    TheShit* theShit;
    if ((theShit = malloc(sizeof(TheShit))) == NULL ) {
        return NULL;
    }
    theShit->name = name;
    theShit->id = getTheShitId(theShit->name);  //TODO
	return theShit;
}


void freeTheShit(TheShit* theShit) {
    if (theShit != NULL) {
        free(theShit->name);
        free(theShit);
    }
}
