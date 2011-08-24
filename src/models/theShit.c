#include <stdlib.h>
#include "models/theShit.h"
#include "utils/vector.h"



//TODO THIS IS WRONG! JUST HARD-CODING! THIS SHOULD RETURN A REFERENCE TO AN
//ALREADY CREATED STRUCTURE. 
TheShit* getTheShit(char* name, Vector* theShit) {
    int i;

    for ( i = 0; i<getVectorSize(theShit); i++ ){
        if ( strcmp(name, ((TheShit*)getFromVector(theShit,i))->name) == 0 ){
            return (TheShit*)getFromVector(theShit,i);
        }
    }
    //this TheShit doesn't exist, let's add it
    TheShit* product;
    if ( (product = malloc(sizeof(TheShit))) == NULL ){
        //TODO Frees!
        return NULL;
    }
    product->id = i;
    product->name = name;
    if ( addToVector(theShit, product) == -1 ){
        //TODO Frees!
        return NULL;
    }
    return product;
}


void freeTheShit(TheShit* theShit) {
    if (theShit != NULL) {
        free(theShit->name);
        free(theShit);
    }
}
