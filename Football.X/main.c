#include "main.h"

void main(void) {
    getResetCause();
    init();
    while(TRUE){
        switch(state){
            case STATE_SLEEP:
                //???
                break;
            case STATE_IDLE:
                runIdle();
                break;
            case STATE_EMERGENCY:
                runEmergency();
                break;
            default:
                //PANIC
                break;
        }
    }
}

void init(void){
    INTDIS;
    //Setup pins
    
    //Setup oscillator
    
    INTCON0bits.IPEN = TRUE;
    INTCON0bits.GIEL = TRUE;
#ifdef LOOPOUT
    TRISAbits.TRISA4 = OUTPUT;
#endif
    INTEN;
    
    //All pins to digital
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    
    memset(systErr.all, 0, SYST_ERR_LEN); //Initialize system error counters
    
    timerInit();
    __delay_ms(400);
    
#ifdef SERIAL_DEBUG
    UART_Init();
    __delay_ms(10);
#endif
    printf("done %d", 2);
#if defined(LOOPOUT) || defined(MSOUT)
    TRISAbits.TRISA0 = OUTPUT;
#endif
    state = STATE_IDLE;
}

void runIdle(void){
    if((unsigned int)(msCount - lastMainRun) < MAIN_LOOP_PER){
        return;
    }
    lastMainRun = msCount;
#ifdef LOOPOUT
    LATAbits.LATA0 = !LATAbits.LATA0;
#endif
    
    //Check if this loop time exceeded MAIN_LOOP_PER
    if((msCount - lastMainRun) > MAIN_LOOP_PER){
        systErr.loopOverruns++;
    }
}

void runEmergency(void){
    
}

void getResetCause(void){
    //Attempt to figure out what caused reset
    if(!PCON1bits.nMEMV){
        //Memory violation
        resetCause = RS_MEM;
        PCON1bits.nMEMV = TRUE;
        return;
    }
    
    if(PCON0bits.STKOVF){
        //Stack overflow
        resetCause = RS_SO;
        PCON0bits.STKOVF = FALSE;
        return;
    }
    
    if(PCON0bits.STKUNF){
        //Stack underflow
        resetCause = RS_SU;
        PCON0bits.STKUNF = FALSE;
        return;
    }
    
    if(!PCON0bits.nWDTWV){
        //WDT window violation
        resetCause = RS_WIN;
        PCON0bits.nWDTWV = TRUE;
        return;
    }
    
    if(!PCON0bits.nRWDT){
        //WDT reset
        resetCause = RS_WDT;
        PCON0bits.nRWDT = TRUE;
        return;
    }
    
    if(!PCON0bits.nRMCLR){
        //MCLR
        resetCause = RS_MCLR;
        PCON0bits.nRMCLR = TRUE;
        return;
    }
    
    if(!PCON0bits.nPOR){
        //Power-on reset
        resetCause = RS_POR;
        PCON0bits.nPOR = TRUE;
        return;
    }
    
    if(!PCON0bits.nBOR){
        //Brownout
        resetCause = RS_BOR;
        PCON0bits.nBOR = TRUE;
        return;
    }
    
}

void __interrupt(irq(default),high_priority) DefaultISR(unsigned char id){
    systErr.unhandledInt++;
    systErr.lastUnhandledInt = id;
}