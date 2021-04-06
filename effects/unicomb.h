
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

static inline float *cb_element(CircularBuffer *cb, ptrdiff_t i);
static inline void cb_decrement(CircularBuffer *cb);
static inline void cb_increment(CircularBuffer *cb);
static inline float redNoise();
static inline void allocateMemory();
static inline float harmonic_noise(unsigned int n, float f);


