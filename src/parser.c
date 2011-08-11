#include <stdio.h>



Airline* parseAirlines() {

    Airline* airline;
    if ( (airLine= malloc(sizeOf(Airline))) == NULL ) {
        return NULL;
    }
    FILE * pFile;
    if ( (pFile = fopen ("empresa.txt","r")) == NULL) {
        return NULL;
    }
    fscanf(pFile,"%d\n",&airline->numberOfPlanes);
    fscanf(pFile,"\n");
   
    Plane* planes;
    if ( (planes = calloc (airline->numberOfPlanes,sizeOf(Plane))) == NULL) {
        return NULL;
    }

}
