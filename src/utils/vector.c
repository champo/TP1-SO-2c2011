#include <stdlib.h>
#include "./vector.h"
#include <pthread.h>

typedef struct Vector {
    void** pointer;
    size_t size;
    pthread_mutex_t lock;
}; 

Vector* create(){
    Vector* vector;
   if ((vector = malloc(sizeof(Vector))) == NULL){    
       return NULL;
    }
   vector->size = 0;
   pthread_mutex_init(&vector->lock, NULL);
   return vector;
}

int add(Vector* vector, void* item){
    pthread_mutex_lock(&vector->lock);
    if (vector->size % 5 == 0){
       if ((vector->pointer = realloc(vector->pointer, (vector->size + 5)*sizeof(void *))) == NULL){
            return -1; 
       } 
    }
    (vector->pointer)[vector->size] = item;
    (vector->size)++;
    pthread_mutex_unlock(&vector->lock);
    return (vector->size) -1;
}

void* get(Vector* vector, int index){
    pthread_mutex_lock(&vector->lock);
    if (index < 0 || index >= vector->size){
        return NULL;
    }
    pthread_mutex_unlock(&vector->lock);
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
