#ifndef POWER_H
#define	POWER_H

#include <xc.h> 
#include "common.h"

#define HP_REG_TRIS TRISDbits.TRISD2
#define HP_REG_LAT LATDbits.LATD2
#define HP_REG_PORT PORTDbits.RD2

#define USB_REG_TRIS TRISAbits.TRISA1
#define USB_REG_LAT LATAbits.LATA1
#define USB_REG_PORT PORTAbits.RA1

#define TRANSITION_INHIBIT_TIME (1000)

unsigned int lastHPRegTransitionTime = 0;
unsigned int lastUSBRegTransitionTime = 0;

void powerInit(void);
void enableHPReg(bool state);
void enableUSBReg(bool state);

inline bool getHPReg(void);
inline bool getUSBReg(void);

#endif

