
#include "C6713dskinit.h"
#define using_bios
extern Uint32 fs; //sampling frequency
extern Uint16 inputsource; //input source (MIC or LINE)

void c6713_dsk_init()
{
    DSK6713_init(); //initialize DSK

    //BSL routine to init DSK
    hAIC23_handle=DSK6713_AIC23_openCodec(0, &config);
    DSK6713_AIC23_setFreq(hAIC23_handle, fs); //set sampling rate

    // choose MIC or LINE IN on AIC23
    DSK6713_AIC23_rset(hAIC23_handle, 0x0004, inputsource);
    MCBSP_config(DSK6713_AIC23_DATAHANDLE,&AIC23CfgData);
    MCBSP_start(DSK6713_AIC23_DATAHANDLE, MCBSP_XMIT_START |
    MCBSP_RCV_START | MCBSP_SRGR_START |
    MCBSP_SRGR_FRAMESYNC, 220); //restart data channel
}

//for communication using polling
void comm_poll()
{
    poll=1; //1 if using polling
    c6713_dsk_init(); //init DSP and codec
}


//for communication using interrupt
void comm_intr()
{
    poll=0; //0 since not polling

    IRQ_globalDisable(); //globally disable interrupts

    c6713_dsk_init(); //init DSP and codec

    CODECEventId=MCBSP_getXmtEventId(DSK6713_AIC23_codecdatahandle);

    #ifndef using_bios
    IRQ_setVecs(vectors);
    #endif

    //if not using DSP/BIOS
    //use interrupt vector table
    //set up in vectors_intr.asm

    IRQ_map(CODECEventId, 11); //map McBSP1 Xmit to INT11
    IRQ_reset(CODECEventId); //reset codec INT 11
    IRQ_globalEnable(); //globally enable interrupts
    IRQ_nmiEnable(); //enable NMI interrupt
    IRQ_enable(CODECEventId); //enable CODEC eventXmit INT11
    output_sample(0); //start McBSP by outputting a sample

}

//output to both channels
void output_sample(int out_data)
{
    short CHANNEL_data;
    AIC_data.uint=0; //clear data structure

    AIC_data.uint=out_data; //write 32-bit data

    //The existing interface defaults to right channel.
    //To default instead to the left channel and use
    //output_sample(short), left and right channels are swapped.
    //In main source program use LEFT 0 and RIGHT 1
    //(opposite of what is used here)

    CHANNEL_data=AIC_data.channel[RIGHT]; //swap channels
    AIC_data.channel[RIGHT]=AIC_data.channel[LEFT];
    AIC_data.channel[LEFT]=CHANNEL_data;

    // if polling, wait for ready to transmit
    if (poll) while(!MCBSP_xrdy(DSK6713_AIC23_DATAHANDLE));
        // write data to AIC23 via MCBSP
        MCBSP_write(DSK6713_AIC23_DATAHANDLE,AIC_data.uint);
}

void output_left_sample(short out_data) //output to left channel
{
    AIC_data.uint=0; //clear data structure
    AIC_data.channel[LEFT]=out_data; //write 16-bit data

    // if polling, wait for ready to transmit
    if (poll) while(!MCBSP_xrdy(DSK6713_AIC23_DATAHANDLE));
        // write data to AIC23 via MCBSP
        MCBSP_write(DSK6713_AIC23_DATAHANDLE,AIC_data.uint);
}
void output_right_sample(short out_data)//output to right channel
{
    AIC_data.uint=0; //clear data structure
    AIC_data.channel[RIGHT]=out_data; //write 16-bit data

    // if polling, wait for ready to transmit
    if (poll) while(!MCBSP_xrdy(DSK6713_AIC23_DATAHANDLE));
        // write data to AIC23 via MCBSP
        MCBSP_write(DSK6713_AIC23_DATAHANDLE,AIC_data.uint);
}

Uint32 input_sample()
{
    short CHANNEL_data; //input from both channels

    // if polling, wait for ready to receive
    if (poll) while(!MCBSP_rrdy(DSK6713_AIC23_DATAHANDLE));
    //read data from AIC23 via MCBSP

    AIC_data.uint=MCBSP_read(DSK6713_AIC23_DATAHANDLE);
    //Swap left and right channels (see comments in output_sample())

    CHANNEL_data=AIC_data.channel[RIGHT]; //swap channels
    AIC_data.channel[RIGHT]=AIC_data.channel[LEFT];
    AIC_data.channel[LEFT]=CHANNEL_data;

    return(AIC_data.uint);
}
