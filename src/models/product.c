#include <stdlib.h>
#include "models/product.h"
#include "utils/vector.h"

#include <string.h>

int getProductId(char* productName, Vector* products) {

    size_t len = getVectorSize(products);
    for (size_t i =0; i < len; i++) {
        Product* product = (Product*) getFromVector(products, i);
        if (strcmp(productName, product->name) ==0){
            return product->id;
        }
    }

    return -1;
}

void getProductName(int id, Vector* products, char* name) {

    Product* product = (Product*) getFromVector(products, id);
    strcpy(name, product->name);
}

Product* getProduct(char* name, Vector* products) {

    int i = getProductId(name, products);

    if (i != -1) {
        return (Product*) getFromVector(products, i);
    }

    //this Product doesn't exist, let's add it
    Product* res;
    if ((res = malloc(sizeof(Product))) == NULL) {
        return NULL;
    }

    res->name = malloc((strlen(name) + 1) * sizeof(char));
    if (res->name == NULL) {
        free(res);
        return NULL;
    }
    strcpy(res->name, name);

    if ((i = addToVector(products, res)) == -1) {
        free(res);
        return NULL;
    }
    res->id = i;

    return res;
}

Product* getProductByID(int id, Vector* product) {
    return (Product*) getFromVector(product, id);
}


void freeProduct(Product* product) {
    if (product != NULL) {
        free(product->name);
        free(product);
    }
}
