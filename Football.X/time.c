#include "time.h"

//TMR0 used for ms counter

void timerInit(void){
#ifdef MSOUT
    TRISAbits.TRISA0 = OUTPUT;
    LATAbits.LATA0 = FALSE;
#endif
    T0CON0bits.T016BIT = FALSE;
    T0CON1bits.T0CS = 0b010; //Fosc/4
    T0CON1bits.T0CKPS = 0b0110; //1/64 prescaler
    TMR0H = MS_TMR_VAL;
    PIE3bits.TMR0IE = TRUE;
    PIR3bits.TMR0IF = FALSE;
    T0CON0bits.EN = TRUE;
    
    
}

//Implement ms timer
void __interrupt(irq(TMR0),high_priority) TMR0ISR(void){
    msCount++;
    
#ifdef MSOUT
    LATAbits.LATA0 = !LATAbits.LATA0;
#endif
    PIR3bits.TMR0IF = FALSE;
}