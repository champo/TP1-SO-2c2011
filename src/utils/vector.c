#include <stdlib.h>
#include "./vector.h"

typedef struct Vector {
    void** pointer;
    size_t size;

}; 

Vector* create(){
    Vector* vector;
   if ((vector = malloc(sizeof(Vector))) == NULL){    
       return NULL;
   }
   vector->size = 0;
   return vector;
}

int add(Vector* vector, void* item){
    if (vector->size % 5 == 0){
       if ((vector->pointer = realloc(vector->pointer, 5*sizeof(void *))) == NULL){
            return -1; 
       } 
    }
    (vector->pointer)[vector->size] = item;
    (vector->size)++;
    return (vector->size) -1;
}

void* get(Vector* vector, int index){
    if (index < 0 || index >= vector->size){
        return NULL;
    }
    return (vector->pointer)[index];
}

void destroy(Vector *vector){
    free(vector->pointer);
    free(vector);
    return;
}

size_t getVectorSize(Vector* vector){
    return vector->size;
} 
