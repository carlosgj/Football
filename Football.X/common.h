#ifndef COMMON_H
#define	COMMON_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <pic18f47q84.h>
#include <stdio.h>

//######### Debug options ###########
#define SERIAL_DEBUG
#ifndef SERIAL_DEBUG
#define printf(x)
#endif

//Uncomment AT MOST ONE of the following to send signals to the test point
//#define LOOPOUT
//#define MSOUT

#define TRUE 1
#define FALSE 0

#define INPUT 1
#define OUTPUT 0

#define _XTAL_FREQ (64000000)

#define SW_VER_STR "0.0.1"

#define INTEN (INTCON0bits.GIE = TRUE)
#define INTDIS (INTCON0bits.GIE = FALSE)

#define SYST_ERR_LEN (3)
union SystErr_t {
    unsigned char all[SYST_ERR_LEN];
    struct{
        unsigned char unhandledInt;
        unsigned char lastUnhandledInt;
        unsigned char loopOverruns;
    };
} systErr;

#define SYST_STAT_LEN (1)
union SystStatus_t{
    unsigned char all[SYST_STAT_LEN];
    struct{
        unsigned char seconds;
    };
} systStat;

enum SystemState{
    STATE_STARTUP = 1,
    STATE_SLEEP = 2,
    STATE_ON = 3,
    STATE_EMERGENCY = 4,
    STATE_UNKNOWN = 0
};

enum ResetCause {
    RS_POR,
    RS_BOR,
    RS_MCLR,
    RS_WDT,
    RS_WIN,
    RS_RST,
    RS_SO,
    RS_SU,
    RS_DP,
    RS_VREG,
    RS_MEM,
    RS_INVALID
};

enum ResetCause resetCause = RS_INVALID;
enum SystemState state = STATE_STARTUP;
unsigned int msCount = 0;
unsigned char txMessage[50];
unsigned char rxMessage[50];

void ItoA(unsigned char val, char* dest);
void fatal(const char * message);

#endif

