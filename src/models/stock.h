#ifndef __STOCK__
#define __STOCK__

#include "models/product.h"
#include "utils/vector.h"



typedef struct {
    int          amount;
    Product*     product;
} Stock;

Stock* initStockById(int productId, int amount, Vector* product);
Stock* initStock(char* name, int amount, Vector* product);
void freeStocks(Vector* stocks);
#endif
