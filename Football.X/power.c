#include "power.h"

void powerInit(void){
    HP_REG_TRIS = OUTPUT;
}

void enableHPReg(bool enable){
//    if(getHPReg()){
//            printf("HP reg is on.\n");
//        }
//        else{
//            printf("HP reg is off.\n");
//        }
    if(getHPReg() == enable){
        return;
    }
    if((msCount - lastHPRegTransitionTime) < TRANSITION_INHIBIT_TIME){
        printf("HP reg transition inhibited due to timeout.\n");
        printf("msCount: %d, LTT: %d\n", msCount, lastHPRegTransitionTime);
    }
    else{
        if(enable){
            printf("Enabling HP reg\n");
        }
        else{
            printf("Disabling HP reg\n");
        }
        HP_REG_LAT = enable;
        lastHPRegTransitionTime = msCount;
    }
}

void enableUSBReg(bool state){
    if(getUSBReg() == state){
        return;
    }
    if((msCount - lastUSBRegTransitionTime) < TRANSITION_INHIBIT_TIME){
        printf("USB reg transition inhibited due to timeout.\n");
    }
    else{
        if(state){
            printf("Enabling USB reg\n");
        }
        else{
            printf("Disabling USB reg\n");
        }
        USB_REG_LAT = state;
        lastUSBRegTransitionTime = msCount;
    }
}

inline bool getHPReg(void){
    return HP_REG_PORT;
}

inline bool getUSBReg(void){
    return USB_REG_PORT;
}
