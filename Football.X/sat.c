#include <xc.h>
#include "sat.h"

void satInit(void) {
    FIFOInit(&satRxBuf);
    
    //Setup UART pins
    ANSELBbits.ANSELB0 = FALSE;
    TRISBbits.TRISB0 = OUTPUT;
    ANSELCbits.ANSELC7 = FALSE;
    TRISCbits.TRISC7 = INPUT;
    //U3RXPPS = 0b00010111; //RC7
    //RB0PPS = 0x20;
    
    //Setup BRG
    U3CON0bits.BRGS = TRUE;
    U3BRGH = 0;
    U3BRGL = 138;
    
    //Setup transmitter
    U3CON0bits.TXEN = TRUE;

    //Setup receiver
    U3CON0bits.RXEN = TRUE;
    
    //Turn on port
    //U3CON1bits.ON = TRUE;
}

void satPeriodic(void){
    
}

void satSendMessage(unsigned char *data, unsigned char len){
    
}