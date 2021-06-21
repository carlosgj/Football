#ifndef GPS_H
#define GPS_H

#include <xc.h> 
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "FIFO.h"

struct DateTime{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct GPSData{
    struct DateTime timestamp;
    float latitude;
    float longitude;
    uint16_t altitude;
    uint8_t nSats;
    uint8_t hdop; //HDOP * 10
};

unsigned char gpsGood = FALSE;
struct GPSData lastFixInfo;

FIFO gpsBuffer;
unsigned char NMEASentenceBuffer[128]; //TBD
unsigned char NMEASentencePtr = 0;

bool sentenceInProgress = FALSE;

unsigned char parsePtr = 6;
unsigned char parseBuf[16];
unsigned char parseBufPtr = 0;


void gpsInit(void);
void gpsCommCheck(void);
void gpsBufferReadTask(void);
void gpsParseSentence(void);
void gpsPeriodic(void);
void processGPSSentence();
void getNextChars(void);
unsigned char dumbAtoI(unsigned char * buf);

#endif

