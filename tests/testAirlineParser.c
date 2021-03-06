#include <stdio.h>
#include "models/city.h"
#include "models/airline.h"
#include "models/plane.h"
#include "utils/vector.h"
#include "parser.h"
#include <stdlib.h>


int testAirlineParser(void) {
    FILE* pFile;
    unsigned int i,j;
    if ( (pFile = fopen ("./configFiles/sim1/empresa2.txt","r")) == NULL) {
        printf("Could not open file\n");
        return -1;
    }
    Airline* airline = parseAirlines(pFile,NULL);
    printf("Numero de  aviones :%d\n",airline->numberOfPlanes);
    for( i = 0; i < airline->numberOfPlanes; i++) {
        printf("Id ciudad origen: %d\n",airline->planes[i].cityId);
        printf("TheShit!\n");
        for(j = 0; j < getVectorSize(airline->planes[i].stocks); j++) { 
            Stock* stock = (Stock*)getFromVector(airline->planes[i].stocks,j);
            printf("%s %d\n",stock->theShit->name,stock->amount);
        }
        printf("\n\n");
    }
    fclose(pFile);
    freeAirline(airline);
	return 0;
}
