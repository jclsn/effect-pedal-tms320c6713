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

double *p_mem = NULL, *delayline;                // memory allocation for length 10
size_t L_delayline = 0, L_delayline_old = 0;

double *first_delay, *last_delay;

double MOD;
double TAP;
double frac;

double x_h, y_uc;

unsigned short i, n;
unsigned short delay_samples;
unsigned short depth_samples;
double modfreq_samples;
unsigned short FB_delay  =  3;                                     // Fixed delay for feedback path

CircularBuffer cbuf;

double * unicomb(double x, unsigned int fs, float modfreq, short modtype, float delay, float depth, float BL, float FF, float FB)
{

    /* Construct delay line */

    if (modtype == SINE)
        delay = depth;

    delay_samples = (unsigned short) round(delay * (float) fs);                // Delay in samples
    depth_samples = (unsigned short) round(depth * (float) fs);                // Width in samples
    L_delayline = 5 + delay_samples + depth_samples * 2;    // Length of the delay in samples

    allocateMemory();


    /* Check for the modulation type: Either a sinusoid or red noise */

    if (modtype == SINE) {

        /* Modulation frequency in samples */

        modfreq_samples  =  modfreq / (float) fs;

        /* Generate sine modulation signal */

        MOD = sin(modfreq_samples * 2.0 * M_PI * n);
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

    x_h = x + FB * *cbuf_element(&cbuf, FB_delay)  ;


    /* Calculate the next output value by applying linear interpolation */

    y_uc = FF * ( frac * *cbuf_element(&cbuf, i + 1) + (1.0 - frac) * *cbuf_element(&cbuf, i) ) + BL * x_h;


    /* Push new intermediate result into the delay line */

    cbuf.current--;
    *cbuf_element(&cbuf, 0) = x_h;

    /* Normalize output by L_infinity norm */

    double L_inf = 1 / abs( 1.0 - 2.1 * (FF + FB + BL));
    filter_out = y_uc * L_inf;

    return &filter_out;
}

double w1[3][2];
double w2[3][2];

double redNoise()
{

    double x_n = (double) rand() / 32768.0;
    double y_n;
    size_t N_sos = 3;
    double sos_redNoise[3][6] = { {3.39488911725442e-05,3.39488911725442e-05,0,1,-0.999362347899025,0},
                                {1,-1.99999423173329,0.999999999869416,1,-1.99865057683769,0.998651487048053},
                                {1,-1.99999752581473,1.00000000013059,1,-1.99968270144378,0.999684007735733}};

    short k;
    for (k = 0; k < N_sos; ++k) {

        y_n     = (w1[k][1] + sos_redNoise[k][0] * x_n);

        w1[k][0] = w2[k][1] + sos_redNoise[k][1] * x_n - sos_redNoise[k][4] * y_n;

        w2[k][0] = sos_redNoise[k][2] * x_n - sos_redNoise[k][5] * y_n;

        w1[k][1] = w1[k][0];

        w2[k][1] = w2[k][0];

        x_n = y_n;

    }

    return y_n;
}


void allocateMemory()
{

    if (!p_mem) {
        p_mem = (double *) calloc(L_delayline, sizeof(double));
        delayline = p_mem;

        L_delayline_old = L_delayline;

        cbuf.array = p_mem;
        cbuf.length = L_delayline;
        cbuf.current = 0;

    }

    if (L_delayline != L_delayline_old) {
        p_mem = (double *) realloc(delayline, L_delayline * sizeof(double));
        cbuf.length = L_delayline;
        L_delayline_old = L_delayline;
    }

    return;
}

double *cbuf_element(CircularBuffer *cbuf, ptrdiff_t i)
{

    ptrdiff_t index = i + cbuf->current;

    if (cbuf->length <= index)
        index -= cbuf->length;

    return &cbuf->array[index];
}

void cbuf_decrement(CircularBuffer *cbuf) {
    cbuf->current--;

    return;
}




