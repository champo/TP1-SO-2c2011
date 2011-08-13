#ifndef __vector__
#define __vector__

typedef struct Vector Vector;

Vector* create();

int add(Vector* vector, void* item);

void* get(Vector* vector, int index);

void destroy(Vector* vector);

size_t getVectorSize(Vector* vector);

#endif
