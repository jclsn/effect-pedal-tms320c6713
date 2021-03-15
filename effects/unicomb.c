/*
 * =======================================================================================================
 *
 *       Filename:  unicomb.c
 *
 *    Description:  A filter structure that is able to generate several effects
 *                  when passed different coefficients.
 *
 *        Version:  1.0
 *        Created:  12.02.2021 12:25:20
 *       Revision:  none
 *       Compiler:  TI C6000 Compiler (v7)
 *
 *         Author:  Jan Clauﬂen (JC), jan.claussen@haw-hamburg.de
 *   Organization:  HAW Hamburg
 *
 *   Effect                   | Blend  | Feedforward | Feedback |   Delay  |   Depth   | Modulation freq
 *   -------------------------|--------|-------------|----------|----------|-----------|---------------
 *   Vibrato                  | 0.0    |   1.0       |   0.0    | 0 ms     | 0-3 ms    | 0.1-5 Hz sine
 *   Flanger                  | 0.7071 |   0.7071    | - 0.7071 | 0 ms     | 0-2 ms    | 0.1-5 Hz sine
 *   Industry standard chorus | 1.0    |   0.7071    |   0.0    | 1-30 ms  | 1-30 ms   | Red noise
 *   White chorus             | 0.7071 |   1.0       |   0.7071 | 1-30 ms  | 1-30 ms   | Red noise
 *   Doubling                 | 0.7071 |   0.7071    |   0.0    | 1-100 ms | 10-100 ms | Red noise
 *   Echo                     | 1.0    |   =< 1.0    | < 1.0    | > 50 ms  |   inf     | Red noise
 *
 * ======================================================================================================
 */

#include "unicomb.h"

double x_h, y_uc;

/* Indexes and delays */

unsigned short i, n;
unsigned short delay_samples;
unsigned short depth_samples;
const unsigned short k_delay  =  3; // Fixed delay for feedback path

/* Intermediate results for the biquad filter */

double w1[3][2];
double w2[3][2];

/* Variables for interpolation */

double modfreq_samples;
double MOD;
double TAP;
double frac;

/* Length of allocated memory and circular buffer structure */

CircularBuffer cb;
size_t L_delayline = 0, L_delayline_old = 0;

double * unicomb(double x, unsigned int fs, float modfreq, short modtype, float delay, float depth, float BL, float FF, float FB)
{


    /* Construct delay line */

    if (modtype == SINE)
        delay = depth;

    delay_samples = (unsigned short) round(delay * 44100.0);                // Delay in samples
    depth_samples = (unsigned short) round(depth * 44100.0);                // Width in samples
    L_delayline = 5 + delay_samples + depth_samples * 2;    // Length of the delay in samples
    allocateMemory();

    /* Check for the modulation type: Either a sinusoid or red noise */

    if (modtype == SINE) {

        /* Modulation frequency in samples */

        modfreq_samples  =  modfreq / 44100.0;

        /* Generate sine modulation signal */

        MOD = sin(modfreq_samples * 2.0 * 3.14 * n);
        n = (n + 1) % fs;
    }

    else {
        double noise = redNoise();
        MOD = noise;
    }

    /* Calculate frac coefficent for interpolation */

    TAP = 1.0 + delay_samples + depth_samples * MOD;
    i   = floor(TAP);
    frac = TAP - i;

    /* Calculate new intermediate value */

    x_h = x + FB * *cb_element(&cb, k_delay)  ;

    /* Calculate the next output value by applying linear interpolation */

    y_uc = FF * ( frac * *cb_element(&cb, i + 1) + (1.0 - frac) * *cb_element(&cb, i) ) + BL * x_h;

    /* Push new intermediate result into the delay line */

    cb.current--;
    *cb_element(&cb, 0) = x_h;

    /* Normalize output by L_infinity norm */

    double L_inf = 1.0 / fabs( 1.0 - 1.0 * (3*FF ));
    filter_out = y_uc * L_inf;

    return &filter_out;
}


double redNoise()
{

    /* Generate random floats and lowpass filter them with a 2-Hz cutoff frequency */

    double x_n = (double) rand() / 32768.0;
    double y_n;
    size_t N_biquad = 3;

    double biquad_redNoise[3][6] = { {3.39488911725442e-05, 3.39488911725442e-05, 0, 1, -0.999362347899025, 0},
                                     {1, -1.99999423173329, 0.999999999869416, 1, -1.99865057683769, 0.998651487048053},
                                     {1, -1.99999752581473, 1.00000000013059, 1, -1.99968270144378, 0.999684007735733}};

    /* Filter the noise with a biquad structure to create red noise */

    short k;

    for (k = 0; k < N_biquad; ++k) {

        y_n     = (w1[k][1] + biquad_redNoise[k][0] * x_n);

        w1[k][0] = w2[k][1] + biquad_redNoise[k][1] * x_n - biquad_redNoise[k][4] * y_n;

        w2[k][0] = biquad_redNoise[k][2] * x_n - biquad_redNoise[k][5] * y_n;

        w1[k][1] = w1[k][0];

        w2[k][1] = w2[k][0];

        x_n = y_n;

    }

    return y_n;
}


void allocateMemory()
{

    /* 
     * Allocate memory for the delayline
     * Check first if memory was already
     */

    if (!L_delayline_old) {
        L_delayline_old = L_delayline;

        //cb.array = calloc(L_delayline, sizeof(double));

        //if(cb.array == NULL)
        //    exit(1);

        cb.length = L_delayline;
        cb.current = 0;

    }

    /* 
     * Check if the paremeters have been changed and the
     * delayline has to be extended (or can be shortened)
     */

    //if (L_delayline != L_delayline_old) {
    //    cb.array = (double *) realloc(cb.array, L_delayline * sizeof(double));
    //    cb.length = L_delayline;
    //    L_delayline_old = L_delayline;
    //}

    return;
}

double *cb_element(CircularBuffer *cb, ptrdiff_t i)
{
    
    /* 
     *  If current value is less that wrap it 
     *  around to the last element 
     */

    if (cb->current < 0)
                cb->current += cb->length;

    /* Calculate the relative index of the array */

    ptrdiff_t index = cb->current + i;

    /* If the relative index is greater than the 
     *      * array size, wrap it around as well */

    if ((size_t) index >= cb->length)
                index -= cb->length;

    return &cb->array[index];

}

void cb_decrement(CircularBuffer *cb)
{
    cb->current--;

    if (cb->current < 0)
        cb->current += cb->length;
}
