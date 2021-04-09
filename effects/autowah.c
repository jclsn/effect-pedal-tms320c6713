/*
 * =====================================================================================
 *
 *       Filename:  autowah.c
 *
 *    Description:  Creates a Wah-wah effect that is controlled by the envelope
 *                  of the input signal
 *
 *        Version:  1.0
 *        Created:  05.02.2021 12:25:20
 *       Revision:  none
 *       Compiler:  TI C6000 Compiler (v7)
 *
 *         Author:  Jan Clauﬂen (JC), jan.claussen@haw-hamburg.de
 *   Organization:  HAW Hamburg
 *
 * =====================================================================================
 */

#include "autowah.h"
#include "smoothing_filter.h"

float v1[4][2] = {{0,0},{0,0}, {0,0}, {0,0}};
float v2[4][2] = {{0,0},{0,0}, {0,0}, {0,0}};

float c = 1.0, c2 = 1.0, d, WIDTH_old = 0.0;
float xh[2] = {0, 0};
float xh_new = 0.0;
float xh_new2 = 0.0;
float ap_y;
float y, x_n, y_n, Wc, fc;
short k;
float Ts = 1 / Fs;
float x_old = 0.0;
float RATE_old = 0.0;
float y_ap2 = 0.0;

float *autowah_sbs (float *x, float WIDTH, float pedalLow, float pedalHigh, float SENSITIVITY, float GAIN, float MIX) {

    /*
     *  y = wahmix-sbs (x, Wc, Wb, MIX)
     *  Applies a sample-by-sample wah filter to the input signal x.
     *  Wc is the normalized center frequency 0<Wc<1, i.e. 2*fc/fs.
     *  Wb is the normalized bandwidth 0<Wb<1, i.e. 2*fb/fs.
     *  MIX is the mixing factor (max = 1)
     */

    envelopeDetection(x, pedalLow, pedalHigh, SENSITIVITY);
    //envelopeDetection2(x, pedalLow, pedalHigh, SENSITIVITY, 8.0);

    Wc = fc * (2.0 * (float) Ts);

    /* Bandpass width parameter calculation */

    if(WIDTH != WIDTH_old) {
        Wb = WIDTH * (2.0 / (float) Fs);
        c = ( tan (M_PI * Wb / 2.0) - 1.0) / ( tan ( M_PI * Wb / 2.0 ) + 1.0 );

        WIDTH_old = WIDTH;
    }

    /* Center frequency parameter calculation */

    d = ( -cos ( M_PI * Wc ));

   /* Calculate next xh value and allpass output */

    xh_new =  ( *x          - d * ( 1.0 - c ) * xh[0] + c * xh[1] );
    ap_y   =  (-c * xh_new + d * ( 1.0 - c ) * xh[0] + xh[1]   );

    /* Add new delay to delay vector and shift old delay by one */

    xh[1] = xh[0];
    xh[0] = xh_new;

    /* Subtract the allpass output from the input to produce a bandpassed output */

    y = GAIN * 0.5 * (*x - ap_y);

    /* Apply mixing factor to input and output */

    *x = *x * (1.0 - MIX);
    y = y * MIX;

    OUT = (*x + y);

    return &OUT;
}


static inline float envelopeDetection(float *x, float pedalLow, float pedalHigh, float SENSITIVITY) {


    /* Envelope detection algorithm */

    x_n = (*x**x) * SENSITIVITY; /* Square the input */

    for (k = 0; k < N_sos_smooth; k++) {

             y_n = v1[k][1] + num_smooth[k][0] * x_n ;

        v1[k][0] = v2[k][1] + num_smooth[k][1] * x_n - den_smooth[k][1] * y_n;

        v2[k][0] = num_smooth[k][2] * x_n - den_smooth[k][2] * y_n;

        v1[k][1] = v1[k][0];

        v2[k][1] = v2[k][0];

        x_n = y_n;
    }

    /* Scale the envelope to calculate and normalize the center frequency */


    fc = pedalLow + pedalHigh *  sqrt(2*y_n);
    //fc = pedalLow + pedalHigh * atan( sqrt(2*y_n)); // Choosing atan to limit freq. at 2600

    return fc;
}


static inline void envelopeDetection2(float *x, float pedalLow, float pedalHigh, float SENSE, float RATE)
{

    if(RATE != RATE_old) {
        c2 = (tan(M_PI * (RATE/Fs)) - 1) / (tan(M_PI * (RATE/Fs)) + 1);
        RATE_old = RATE;
    }
    x_n = *x **x;

    xh_new2 = x_n - c2*x_old;
    y_ap2 = c2 * xh_new2 + x_old;
    x_old = xh_new2;
    y_n = 0.5 * (x_n + y_ap2);

    fc = pedalLow + pedalHigh *  sqrt(2*y_n);
    //fc = pedalLow + pedalHigh * atan( sqrt(2*y_n)); // Choosing atan to limit freq. at 2600
}

