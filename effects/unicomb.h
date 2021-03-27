
#include <stddef.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "../main.h"

/* This struct implements a circular buffer which can be accessed by cb_element() */

typedef struct
{
    float array[13232];
    size_t length;
    ptrdiff_t current;

} CircularBuffer;


/* Function prototypes */

float *cb_element(CircularBuffer *cb, ptrdiff_t i);
void cb_decrement(CircularBuffer *cb);
void cb_increment(CircularBuffer *cb);
float redNoise();
void allocateMemory();

float harmonic_noise(unsigned int n, float f);


