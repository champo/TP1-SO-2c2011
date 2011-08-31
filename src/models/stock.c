#include "models/theShit.h"
#include <stdlib.h>
#include "utils/vector.h"
#include "models/stock.h"



Stock* initStock(char* name, int amount, Vector* theShit) {

    Stock* stock;
    if ((stock = malloc(sizeof(Stock))) == NULL) {
        return NULL;
    }
    stock->amount = amount;
    stock->theShit = getTheShit(name, theShit); 
    return stock;
}

void freeStocks(Vector* stocks) {
    unsigned int i;
    unsigned int size =  getVectorSize(stocks);

    if (stocks != NULL) {
        for (i = 0; i < size; i++) {
			free(getFromVector(stocks,i));
		}
        destroyVector(stocks);
    }
}
