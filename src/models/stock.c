#include "models/theShit.h"
#include <stdlib.h>
#include "utils/vector.h"
#include "models/stock.h"



Stock* initStock(char* name, int amount) {

    Stock* stock;
    if ((stock = malloc(sizeof(Stock))) == NULL) {
        return NULL;
    }
    stock->amount = amount;
    stock->theShit = getTheShit(name); //TODO get a reference of theShit.
    return stock;
}

void freeStocks(Vector* stocks) {
    unsigned int i;
    unsigned int size =  getVectorSize(stocks);
    if (stocks != NULL) {
        for (i = 0; i < size; i++) {
            //freeTheShit(((Stock*) getFromVector(stocks,i))->theShit); //TODO do nothing?
			free(getFromVector(stocks,i));
		}
        destroyVector(stocks);
    }
}