#ifndef GPS_H
#define GPS_H

#include <xc.h> 
#include "common.h"
#include "FIFO.h"

struct DateTime{
    uint16_t year;
    uint8_t month;
    uint16_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct GeoPosition{
    int16_t intLatitude;
    uint16_t fracLatitude;
    int16_t intLongitude;
    uint16_t fracLongitude;
    uint16_t altitude;
};

struct GPSMetaData{
    uint8_t quality;
    uint8_t nSats;
    uint8_t hdop; //HDOP * 10
};

unsigned char gpsGood;
struct DateTime lastFixTime;
struct GeoPosition lastFixPosition;
struct GPSMetaData lastFixInfo;

FIFO gpsBuffer;
unsigned char NMEASentenceBuffer[100]; //TBD

void gpsInit(void);
void gpsCommCheck(void);
void gpsBufferReadTask(void);
void gpsParseSentence(void);
void gpsPeriodic(void);

#endif

