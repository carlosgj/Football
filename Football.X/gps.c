#include <xc.h>
#include "gps.h"

void gpsInit(void) {
    FIFOInit(&gpsBuffer);
    
    //Setup UART pins
    ANSELBbits.ANSELB0 = FALSE;
    TRISBbits.TRISB0 = OUTPUT;
    ANSELCbits.ANSELC7 = FALSE;
    TRISCbits.TRISC7 = INPUT;
    //U2RXPPS = 0b00010111; //RC7
    //RB0PPS = 0x20;
    
    //Setup BRG
    U2CON0bits.BRGS = TRUE;
    U2BRGH = 0;
    U2BRGL = 138;
    
    //Setup transmitter
    U2CON0bits.TXEN = TRUE;

    //Setup receiver
    U2CON0bits.RXEN = TRUE;
    
    //Turn on port
    //U2CON1bits.ON = TRUE;
    
}

void gpsPeriodic(void){
    
}

void __interrupt(irq(U2RX),high_priority) GPSRxISR(void){
    FIFOPush(&gpsBuffer, U1RXB);
}