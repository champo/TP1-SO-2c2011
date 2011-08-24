#ifndef __VECTOR__
#define __VECTOR__

#include <stddef.h>

typedef struct Vector Vector;

Vector* createVector(void);

int addToVector(Vector* vector, void* item);

void* getFromVector(Vector* vector, unsigned int index);

void destroyVector(Vector* vector);

size_t getVectorSize(Vector* vector);

#endif
