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

#include <math.h>

#include "autowah.h"
#include "../main.h"

double s1[3][2];
double s2[3][2];
double sos_smooth[2][6] = {{0.00718491019141383,  0.00718491019141383, 0,                   1, -0.999725227879379, 0},
                          {0.00718491019141383, -0.0143698191841365 , 0.00718491019141383, 1, -1.99982839868153,  0.999828461367708}};
short N_sos = 2;
double c = 1.0, d, c_old = 0.0;
double xh[2] = {0, 0};
double xh_new = 0.0;
double ap_y;
double y, x_n, y_n, Wc, fc;
short k;


double* autowah_sbs (double x, double Wb, double MIX, int fs) {

    /*
     *  y = wahmix-sbs (x, Wc, Wb, MIX)
     *  Applies a sample-by-sample wah filter to the input signal x.
     *  Wc is the normalized center frequency 0<Wc<1, i.e. 2*fc/fs.
     *  Wb is the normalized bandwidth 0<Wb<1, i.e. 2*fb/fs.
     *  MIX is the mixing factor (max = 1)
     */

    /* Envelope detection algorithm */

    x_n = x*x; /* Square the input */

    for (k = 0; k < N_sos; k++) {

             y_n = s1[k][1] + sos_smooth[k][0] * x_n ;

        s1[k][0] = s2[k][1] + sos_smooth[k][1] * x_n - sos_smooth[k][4] * y_n;

        s2[k][0] = sos_smooth[k][2] * x_n - sos_smooth[k][5] * y_n;

        s1[k][1] = s1[k][0];

        s2[k][1] = s2[k][0];

        x_n = y_n;
    }

    /* Scale the envelope to calculate and normalize the center frequency */

    //fc = 100 + y_n * 10000.0;

    fc = 200 + 2500 * atan(25 * y_n);

    Wc = fc * (2.0 / 44100.0);

    /* Bandpass width parameter calculation */

    if(c != c_old) {
        c = ( tan (M_PI * Wb / 2.0) - 1.0) / ( tan ( M_PI * Wb / 2.0 ) + 1.0 );
        c_old = c;
    }

    /* Center frequency parameter calculation */

    d = ( -cos ( M_PI * Wc ));

   /* Calculate next xh value and allpass output */

    xh_new = (double) ( x          - d * ( 1.0 - c ) * xh[0] + c * xh[1] );
    ap_y   = (double) (-c * xh_new + d * ( 1.0 - c ) * xh[0] + xh[1]   );

    /* Add new delay to delay vector and shift old delay by one */

    xh[1] = xh[0];
    xh[0] = xh_new;

    /* Subtract the allpass output from the input to produce a bandpassed output */

    y = 0.5 * (x - ap_y);

    /* Apply mixing factor to input and output */

    x = x * (1.0 - MIX);
    y = y * MIX;

    filter_out = x + y;

    return &filter_out;
}

