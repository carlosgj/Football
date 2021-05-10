#include <xc.h>
#include "UI.h"

void UIInit(void){
    
}

void updateDisplay(void){
    
}

void checkSwitches(void){
    
}

void updateSwitch(Switch *sw, unsigned char value){
    sw->currentValue = value;
    if((msCount - sw->lastTransition) < SW_DEBOUNCE_TIME){
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