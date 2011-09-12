#ifndef __PRODUCT__
#define __PRODUCT__

#include "utils/vector.h"

typedef struct {
    int     id;
    char*   name;
} Product;

int getProductId(char* productName, Vector* product);

void getProductName(int id, Vector* product, char* name);

Product* getProduct(char* name, Vector* product);

Product* getProductByID(int id, Vector* product);

void freeProduct(Product* product);

#endif
