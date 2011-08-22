#include "models/theShit.h"
#include <stdlib.h>
#include "utils/vector.h"
#include "models/stock.h"





Stock* initStock(char* name, int amount) {

    Stock* stock;
    TheShit* theShit;
    if ( (stock = malloc(sizeof(Stock))) == NULL ) {
        return NULL;
    }
    stock->amount = amount;
	if ((theShit = initTheShit(name)) == NULL) {
		free(stock);
		return NULL;
	}
	
    stock->theShit = theShit;
    return stock;
}

void freeStock(Vector* stocks) {
    unsigned int i;
    if ( stocks != NULL) {
        for ( i = 0; i < getVectorSize(stocks); i++) {
            freeTheShit(((Stock*) getFromVector(stocks,i))->theShit);
			free(getFromVector(stocks,i));
		}
        destroyVector(stocks);
    }
}
