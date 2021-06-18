#include <xc.h>
#include "UI.h"

void UIInit(void){
    UI_SWITCH_TRIS = INPUT;
    UI_SWITCH_WPU = TRUE;
    
    //Setup interrupts
    IOCBNbits.IOCBN4 = TRUE;
    IOCBPbits.IOCBP4 = TRUE;
}

void updateDisplay(void){
    
}

void checkSwitches(void){
    updateSwitch(&UISwitch, !UI_SWITCH_PORT);
}

void updateSwitch(Switch *sw, unsigned char value){
    sw->currentValue = value;
    if((msCount - sw->lastTransition) < SW_DEBOUNCE_TIME && state != STATE_SLEEP){
        //We're in the debounce time
        //Do nothing
    }
    else{
        if(sw->debouncedValue != sw->currentValue){
            //This is a transition
            sw->debouncedValue = sw->currentValue;
            sw->lastTransition = msCount;
        }
    }
}

void __interrupt(irq(IOC), low_priority) UI_Sw_N_ISR(void){
    if(IOCBFbits.IOCBF4){
        IOCBFbits.IOCBF4 = FALSE;
        printf("IOC on UI sw\n");
        return;
    }
    
    printf("IOC!\n");
    
}
