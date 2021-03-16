

#define M_PI 3.1415926535897931
#define SINE 0
#define NOISE 1

#define AUTOWAH  1
#define VIBRATO  2
#define FLANGER  4
#define CHORUS   8
#define DOUBLING 16

extern short effect;
extern float OUT;
extern unsigned int Fs;

float *autowah_sbs (float *x, float Wb, float MIX);
float *unicomb(float *x, float modfreq, short modtype, float delay, float depth, float BL, float FF, float FB);
