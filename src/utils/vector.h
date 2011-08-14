#ifndef __vector__
#define __vector__

typedef struct Vector Vector;

Vector* createVector();

int addToVector(Vector* vector, void* item);

void* getFromVector(Vector* vector, int index);

void destroyVector(Vector* vector);

size_t getVectorSize(Vector* vector);

#endif
