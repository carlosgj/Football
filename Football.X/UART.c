#include <xc.h>
#include "UART.h"

void UART_Init(void){
    
    //Setup UART pins
    ANSELBbits.ANSELB0 = FALSE;
    TRISBbits.TRISB0 = OUTPUT;
    ANSELCbits.ANSELC7 = FALSE;
    TRISCbits.TRISC7 = INPUT;
    U1RXPPS = 0b00010111; //RC7
    RB0PPS = 0x20;
    
    //Setup BRG
    U1CON0bits.BRGS = TRUE;
    U1BRGH = 0;
    U1BRGL = 138;
    
    //Setup transmitter
    U1CON0bits.TXEN = TRUE;

    //Setup receiver
    U1CON0bits.RXEN = TRUE;
    
    //Turn on port
    U1CON1bits.ON = TRUE;
}

void putch(unsigned char theByte){
    U1TXB = theByte;
    while(!U1ERRIRbits.TXMTIF){
    }
}