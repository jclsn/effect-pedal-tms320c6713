
/* Function prototpye for noise generator function */

#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "../main.h"

typedef struct
{
    double *array;
    size_t length;
    ptrdiff_t current;

} CircularBuffer;

double redNoise();
void allocateMemory();
double *cbuf_element(CircularBuffer *cbuf, ptrdiff_t i);
void cbuf_decrement(CircularBuffer *cbuf);

