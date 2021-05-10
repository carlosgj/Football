#ifndef UI_H
#define	UI_H

#include <xc.h> 
#include "common.h"

#define SW_DEBOUNCE_TIME (100)

#define EMERGENCY_SWITCH_PIN
#define EMERGENCY_SWITCH_TRIS

#define SQUAWK_SWITCH_PIN
#define SQUAWK_SWITCH_TRIS

#define UI_SWITCH_PIN
#define UI_SWITCH_TRIS

#define TX_SWITCH_PIN
#define TX_SWITCH_TRIS

#define RX_SWITCH_PIN
#define RX_SWITCH_TRIS

typedef struct Switch_t {
    unsigned currentValue :1;
    unsigned debouncedValue :1;
    unsigned RESERVED :6;
    unsigned int lastTransition;
} Switch;

void UIInit(void);
void updateDisplay(void);
void checkSwitches(void);
void updateSwitch(Switch* sw, unsigned char value);

Switch emergencySwitch;
Switch squawkSwitch;
Switch txSwitch;
Switch rxSwitch;
Switch UISwitch;

#endif	/* XC_HEADER_TEMPLATE_H */

