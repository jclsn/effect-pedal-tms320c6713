/*
 * =====================================================================================
 *
 *       Filename:  main_effect_pedal.c
 *
 *    Description:  Main of the effect pedal operating system
 *
 *        Version:  1.0
 *        Created:  05.02.2021 12:15:03
 *       Revision:  none
 *       Compiler:  TI C6000 Compiler (v7)
 *
 *         Author:  Jan Clauﬂen (JC), jan.claussen@haw-hamburg.de
 *   Organization:  HAW Hamburg
 *
 * =====================================================================================
 */

#define LEFT 1
#define RIGHT 0
#include <stdio.h>              //stdio library
#include <stdlib.h>             //stdlib library
#include <math.h>               //math library
#include <csl_mcbsp.h>              // for MCBSP_read(...) and MCBSP_write(...)
#include <math.h>

/* Libraries for DSK board */

#include "c6713dskinit.h"       //codec-DSK support file
#include "dsk6713.h"
#include "dsk6713_aic23.h"

/* Local libraries */

#include "main.h"

/* global variables for DSK board version */

extern MCBSP_Handle DSK6713_AIC23_DATAHANDLE;
static Uint32 CODECEventId;
Uint32 fs=DSK6713_AIC23_FREQ_44KHZ;     //for sampling frequency

/* this union allows to store left and right channel 16 bit samples in a 32 bit int */

typedef union {
    unsigned int both;
    short channel[2];
} AIC23_DATA;

AIC23_DATA AIC23_data;

/* Function prototype */

double* autowah_sbs (double x, double Wb, double MIX, int fs);
double * unicomb(double x, unsigned int fs, double modfreq, short modtype, float delay, float depth, float BL, float FF, float FB);

/* Global variables */

double new_sample;
double filter_out;
short fb = 200;               // Width of passband in Hz
double Wb =  200.0 * 2.0 / 44100.0;       // Calculate normalized passband width
double MIX = 1.0;

#define SINE 0
#define NOISE 1

interrupt 
void intser_McBSP1()
{
    /* Read date from line or mic input */

	AIC23_data.both = MCBSP_read(DSK6713_AIC23_DATAHANDLE);

	/* Convert new sample to float */

	new_sample = (double) AIC23_data.channel[LEFT] / 32768;


	/* ------------------------ Effects are applied here ----------------------------- */

	/* Apply Autowah sample-by-sample */

    //filter_out = *autowah_sbs(new_sample, Wb, MIX, fs);

	/* Apply Vibrato filter sample-by-sample */

    //filter_out = *unicomb(new_sample, 44100.0, 5.0, SINE, 0.0, 0.001, 0.0, 1.0, 0.0);

	/* Apply Flanger filter sample-by-sample */

    filter_out = *unicomb(new_sample, 44100.0, 0.1, SINE, 0.015, 0.015, 0.7071, 0.7071, -0.7071);

	/* Apply Chorus filter sample-by-sample */

    //filter_out = *unicomb(new_sample, 44100.0, 5.0, NOISE, 0.002, 0.002, 0.7071, 1.0, 0.7071);

	/* Apply doubling filter sample-by-sample */

    //filter_out = *unicomb(new_sample, 44100.0, 2.0, NOISE, 0.075, 0.075, 0.7071, 0.7071, 0.0);


	/* ------------------------ Effects are done here ----------------------------- */


    /* Write filter output to AIC23 strucute, left and right channel */

    AIC23_data.channel[LEFT] = (short) floor(filter_out * 32768.0);
    AIC23_data.channel[RIGHT] = (short) floor(filter_out * 32768.0);;

    /* Write data to line and headphone output */

	MCBSP_write(DSK6713_AIC23_DATAHANDLE, AIC23_data.both);

	return;
}

void main()
{
    IRQ_globalDisable();                //disable interrupts
    DSK6713_init();                     //call BSL to init DSK-EMIF,PLL)

    // handle(pointer) to codec
    hAIC23_handle=DSK6713_AIC23_openCodec(0, &config);
    DSK6713_AIC23_setFreq(hAIC23_handle, fs);  //set sample rate

    //interface 32 bits toAIC23
    MCBSP_config(DSK6713_AIC23_DATAHANDLE,&AIC23CfgData);

    //start data channel
    MCBSP_start(DSK6713_AIC23_DATAHANDLE, MCBSP_XMIT_START | MCBSP_RCV_START |
        MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220);

    CODECEventId= MCBSP_getXmtEventId(DSK6713_AIC23_DATAHANDLE);//McBSP1 Xmit
    IRQ_map(CODECEventId, 5);       //map McBSP1 Xmit to INT5
    IRQ_reset(CODECEventId);        //reset codec INT5
    IRQ_globalEnable();             //globally enable interrupts
    IRQ_nmiEnable();                //enable NMI interrupt
    IRQ_enable(CODECEventId);       //enable CODEC eventXmit INT5
    IRQ_set(CODECEventId);              //manually start the first interrupt

    while(1){

    }
}
