

#include "../main.h"
#include <math.h>

extern float y, Wb;

static inline float envelopeDetection(float *x, float pedalLow, float pedalHigh, float SENSITIVITY);
