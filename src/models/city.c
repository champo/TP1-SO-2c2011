#include "utils/vector.h"
#include "models/stock.h"
#include "models/city.h"

void freeCity(City* city){

    freeStocks(city->stock);
    free(city->name);
    free(city);
    return;
}

int getCityId(const char* cityName, Vector* cities) {

    size_t len = getVectorSize(cities);
    for (size_t i = 0; i < len; i++ ) {

        City* city = (City*) getFromVector(cities, i);
        if (strcmp(cityName, city->name) == 0 ){
            return city->id;
        }
    }

    return -1;
}

