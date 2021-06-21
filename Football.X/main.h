#ifndef MAIN_H
#define MAIN_H

#include <xc.h>
#include <string.h>
#include "config.h"
#include "common.h"
#include "UART.h"
#include "time.h"
#include "sat.h"
#include "gps.h"
#include "UI.h"
#include "power.h"

#define MAIN_LOOP_PER (10)

unsigned int lastMainRun = 0;

unsigned int asyncTlmCounter = 0;

void init(void);
void periodicTasks();
void runOn(void);
void runSleep(void);
void runEmergency(void);
void processCommand(void);
void getResetCause(void);
void setState(enum SystemState newState);
#endif