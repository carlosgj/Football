#include "main.h"

void main(void) {
    getResetCause();
    init();
    while(TRUE){
        periodicTasks();
        switch(state){
            case STATE_SLEEP:
                runSleep();
                break;
            case STATE_ON:
                runOn();
                break;
            case STATE_EMERGENCY:
                runEmergency();
                break;
            default:
                fatal("Invalid state.\n");
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
    __delay_ms(500);
    
#ifdef SERIAL_DEBUG
    UART_Init();
    __delay_ms(10);
#endif
    printf("Serial initialized.\n");
    printf("SW ver: %s\n", SW_VER_STR);
    
    printf("Initializing power control\n");
    powerInit();
    printf("Power control initialized.\n");
    
    printf("Initializing UI\n");
    UIInit();
    printf("UI initialized.\n");
    
    printf("Initializing SAT\n");
    satInit();
    printf("SAT initialized.\n");
    
    printf("Initializing GPS\n");
    gpsInit();
    printf("GPS initialized.\n");
    
    printf("Initialization done.\n");
#if defined(LOOPOUT) || defined(MSOUT)
    TRISAbits.TRISA0 = OUTPUT;
#endif
    setState(STATE_ON);
}

// This function runs on EVERY main loop, regardless of state. 
// One of the main things it does is to monitor certain switches 
// to see if we need to transition to a different mode. 
void periodicTasks(){
#ifdef LOOPOUT
    LATAbits.LATA0 = !LATAbits.LATA0;
#endif
    if(state == STATE_SLEEP){
        msCount += 1000; //Assume we've been asleep for at least a second
    }
    checkSwitches();
    if(emergencySwitch.debouncedValue){
        setState(STATE_EMERGENCY);
    }
    else{
        if(UISwitch.debouncedValue){
            setState(STATE_ON);
        }
        else{
            setState(STATE_SLEEP);
        }
    }
    //Memory scan increment
}

void runOn(void){
    enableHPReg(TRUE);
    gpsPeriodic();
    satPeriodic();
    if(UISwitch.debouncedValue){
        updateDisplay();         
    }
}

void runSleep(void){
    enableHPReg(FALSE);
    printf("Sleeping.\n");
    SLEEP();
    asm("NOP");
}

void runEmergency(void){
    
}

void setState(enum SystemState newState){
    //printf("Transition from %d to %d requested\n", state, newState);
    if(newState == state){
        return;
    }
    switch(state){
        case STATE_STARTUP:
            if(newState == STATE_ON){
                //This is expected to happen once per boot
                state = STATE_ON;
                printf("Transitioning from STATE_STARTUP to STATE_ON.\n");
            }
            else{
                printf("Illegal state transition requested: STATE_STARTUP to %d\n", newState);
            }
            break;
        case STATE_SLEEP:
            switch(newState){
                case STATE_EMERGENCY:
                    printf("Emergency mode requested.\n");
                case STATE_ON:
                    printf("Transitioning from STATE_SLEEP to STATE_ON.\n");
                    state = newState;
                    //Do wakeup timer stuff
                    break;
                default:
                    printf("Illegal state transition requested: STATE_SLEEP to %d\n", newState);
            }
            break;
        case STATE_EMERGENCY:
            //verify that emergency switch isn't on
            if(emergencySwitch.debouncedValue || txInProgress){
                //Switch is still on 
                printf("Transition from STATE_EMERGENCY to %d disallowed while emergency switch is on.\n", newState);
            }
            else{
                switch(newState){
                    case STATE_ON:
                    case STATE_SLEEP:
                        printf("Transitioning from STATE_EMERGENCY to %d\n", newState);
                        state = newState;
                        break;
                    default:
                        printf("Illegal state transition requested: STATE_EMERGENCY to %d\n", newState);
                        break;
                }
            }
            break;
        case STATE_ON:
            switch(newState){
                case STATE_SLEEP:
                    printf("Transitioning from STATE_ON to STATE_SLEEP\n");
                    state = newState;
                    break;
                case STATE_EMERGENCY:
                    printf("Emergency mode requested.\n");
                    state = newState;
                    break;
                default:
                    printf("Illegal state transition requested: STATE_ON to %d\n", newState);
                    break;
            }
            break;
        case STATE_UNKNOWN:
        default:
            fatal("Current state invalid.\n");
            break;
    }
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