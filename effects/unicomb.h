
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "../main.h"

/* This struct implements a circular buffer which can be accessed by cb_element() */

typedef struct
{
    double array[1000];
    size_t length;
    ptrdiff_t current;

} CircularBuffer;


/* Function prototypes */

double *cb_element(CircularBuffer *cb, ptrdiff_t i);
void cb_decrement(CircularBuffer *cb);
double redNoise();
void allocateMemory();



