

/* Define PI as it is not included in Math.h */
#define M_PI 3.1415926535897931

/* Effect types */

#define AUTOWAH  1
#define VIBRATO  2
#define FLANGER  4
#define CHORUS   8
#define DOUBLING 16

/* Modulation type for Unicomb filter */

#define SINE 0
#define NOISE 1

/* Global variables */

extern short effect;
extern float IN, OUT;
extern unsigned int Fs;

/* Function prototype */

float *autowah_sbs (float *x, float Wb, float MIX);
float *unicomb(float *x, float modfreq, short modtype, float delay, float depth, float BL, float FF, float FB);

