#include <stdlib.h>
#include "models/theShit.h"
#include "utils/vector.h"


int getTheShitId(char* theShitName, Vector* theShit) {

    size_t len = getVectorSize(theShit);
    for (size_t i =0; i < len; i++) {

        TheShit* shit = (TheShit*) getFromVector(theShit, i);
        if (strcmp(theShitName, shit->name) ==0){
            return shit->id;
        }
    }

    return -1;
}

TheShit* getTheShit(char* name, Vector* theShit) {

    int i = getTheShitId(name, theShit);

    if (i != -1) {
        return (TheShit*) getFromVector(theShit, i);
    }

    //this TheShit doesn't exist, let's add it
    TheShit* product;
    if ((product = malloc(sizeof(TheShit))) == NULL) {
        return NULL;
    }

    product->name = name;
    if ((i = addToVector(theShit, product)) == -1) {
        free(product);
        return NULL;
    }
    product->id = i;

    return product;
}


void freeTheShit(TheShit* theShit) {
    if (theShit != NULL) {
        free(theShit->name);
        free(theShit);
    }
}
