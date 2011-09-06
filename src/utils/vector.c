#include <stdlib.h>
#include "./utils/vector.h"
#include <pthread.h>

struct Vector {
    void** pointer;
    size_t size;
    pthread_mutex_t lock;
};

Vector* createVector(void){
    Vector* vector;
    if ((vector = malloc(sizeof(Vector))) == NULL){
        return NULL;
    }
    vector->pointer = NULL;
    vector->size = 0;
    pthread_mutex_init(&vector->lock, NULL);
    return vector;
}

int addToVector(Vector* vector, void* item){
    int ans;

    pthread_mutex_lock(&vector->lock);
    if (vector->size % 5 == 0){
        if ((vector->pointer = realloc(vector->pointer, (vector->size + 5)*sizeof(void *))) == NULL) {
            free(vector->pointer);
            return -1;
        }
    }
    (vector->pointer)[vector->size] = item;
    (vector->size)++;
    ans = (vector->size) - 1;
    pthread_mutex_unlock(&vector->lock);
    return ans;
}

void* getFromVector(Vector* vector, unsigned int index){
    void* ans;

    pthread_mutex_lock(&vector->lock);
    if (index >= vector->size){
        return NULL;
    }
    ans = (vector->pointer)[index];
    pthread_mutex_unlock(&vector->lock);
    return ans;
}

void destroyVector(Vector *vector){
    free(vector->pointer);
    free(vector);
    return;
}

size_t getVectorSize(Vector* vector){
    return vector->size;
}
