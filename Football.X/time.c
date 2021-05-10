#include "time.h"

//TMR0 used for ms counter

void timerInit(void){
#ifdef MSOUT
    TRISAbits.TRISA0 = OUTPUT;
    LATAbits.LATA0 = FALSE;
#endif
    
    //Slow clock (TMR0)
    T0CON0bits.T016BIT = FALSE;
    T0CON1bits.T0CS = 0b100; //LFINTOSC
    T0CON1bits.ASYNC = TRUE;
    T0CON1bits.T0CKPS = 0b1111;
    TMR0H = 10;
    PIE3bits.TMR0IE = TRUE;
    PIR3bits.TMR0IF = FALSE;
    T0CON0bits.EN = TRUE;
    
    //MS clock (TMR1)
    T1CLKbits.CS = 0b00001; //Fosc/4
    T1CONbits.CKPS = 0b11; //8x prescaler
    //T1CONbits.RD16 = TRUE;
    TMR1H = (unsigned char)(MS_TMR_VAL >> 8);
    TMR1L = (unsigned char)(MS_TMR_VAL & 0xff);
    PIE3bits.TMR1IE = TRUE;
    PIR3bits.TMR1IF = FALSE;
    T1CONbits.ON = TRUE;
    
    
}

//Implement slow clock
void __interrupt(irq(TMR0), low_priority) TMR0ISR(void){
    OSCFRQ = 0b1000;
    PIR3bits.TMR0IF = FALSE;
}

//Implement ms counter
void __interrupt(irq(TMR1), high_priority) TMR1ISR(void){
    msCount++;
    TMR1H = (unsigned char)(MS_TMR_VAL >> 8);
    TMR1L = (unsigned char)(MS_TMR_VAL & 0xff);
#ifdef MSOUT
    LATAbits.LATA0 = !LATAbits.LATA0;
#endif
    PIR3bits.TMR1IF = FALSE;
}