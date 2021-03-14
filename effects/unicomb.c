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

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "../main.h"
#include "unicomb.h"

unsigned short n;
double *delayline, *p_delayline;                // memory allocation for length 10
double *first_delay, *last_delay;
double modfreq_samples;
double MOD;
double TAP;
int i;
double frac;

double w1[3][2];
double w2[3][2];
double x_h, y_uc;
unsigned int L_delayline = NULL;
int delay_samples;
int depth_samples;
unsigned int FB_delay  =  3;                                     // Fixed delay for feedback path

double * unicomb(double x, unsigned int fs, double modfreq, short modtype, float delay, float depth, float BL, float FF, float FB) {


    /* Construct delay line */

    if (modtype == 0)
        delay = depth;

    delay_samples   = round(delay * fs);                // Delay in samples
    depth_samples   = round(depth * fs);                // Width in samples
    L_delayline = 5 + delay_samples + depth_samples * 2;    // Length of the delay in samples

    if (!delayline) {
        delayline = (double *) calloc(L_delayline, sizeof(double));
        first_delay = &delayline[0];
        last_delay = &delayline[L_delayline -1];
    }

    /* Check for the modulation type: Either a sinusoid or red noise */

    if (modtype == 0) {

        /* Modulation frequency in samples */

        modfreq_samples  =  modfreq / fs;

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

    x_h = x + FB * delayline[FB_delay % L_delayline]  ;


    /* Calculate the next output value by applying linear interpolation */

    y_uc = FF * ( frac * delayline[(i + 1) % L_delayline] + (1.0 - frac) * delayline[i % L_delayline] ) + BL * x_h;


    /* Push new intermediate result into the delay line */

    if (--delayline < first_delay)
        delayline = last_delay;

    delayline[0] = x_h;

    /* Normalize output by L_infinity norm */

    double L_inf = 1 / abs( 1.0 - 2.1 * (FF + FB + BL));
    filter_out = y_uc * L_inf;

    return &filter_out;
}

double redNoise() {

    double x_n = (double) rand() / 32768.0;
    double y_n;
    short N_sos = 3;
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
