#include "utils/vector.h"
#include "models/stock.h"
#include "models/city.h"

void freeCity(City* city){

    freeStocks(city->stock);
    free(city->name);
    free(city);
    return;
}
