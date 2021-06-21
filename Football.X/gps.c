#include <xc.h>
#include "gps.h"

void gpsInit(void) {
    FIFOInit(&gpsBuffer);
    
    //Setup UART pins
    ANSELBbits.ANSELB5 = FALSE;
    TRISBbits.TRISB5 = INPUT;
    U2RXPPS = 0b001101; //RB5
    
    //Setup BRG
    U2CON0bits.BRGS = TRUE;
    U2BRGH = 0x6;
    U2BRGL = 0x82;
    
    //Setup transmitter
    U2CON0bits.TXEN = FALSE;

    //Setup receiver
    U2CON0bits.RXEN = TRUE;
    
    //Turn on port
    U2CON1bits.ON = TRUE;
    
    PIE8bits.U2RXIE = TRUE;
    
}

void gpsPeriodic(void){
    while(FIFOGetUsed(&gpsBuffer) > 0){
        //New chars in buffer
        unsigned char newChar;
        FIFOPop(&gpsBuffer, &newChar);
        if(sentenceInProgress){
            if(newChar == '$'){
                //Unexpected start of sentence
                printf("Unexpected start of sentence\n");
                NMEASentencePtr = 0;
            }
            
            NMEASentenceBuffer[NMEASentencePtr++] = newChar;
            
            if(newChar == 10){
                //Newline
                NMEASentenceBuffer[NMEASentencePtr] = 0;
                sentenceInProgress = FALSE;
                processGPSSentence();
                NMEASentencePtr = 0;
            }
            
        }
        else{
            //Sentence not in progress
            if(newChar == '$'){
                NMEASentencePtr = 0;
                NMEASentenceBuffer[NMEASentencePtr++] = newChar;
                sentenceInProgress = TRUE;
            }
            else{
                //Unexpected character
                printf("Got 0x%x when expecting start of sentence\n", newChar);
            }
        }
    }
}

void processGPSSentence(void){
    //printf("%s", NMEASentenceBuffer);
    
    //Validation
    if(NMEASentenceBuffer[0] != '$'){
        printf("NMEA error: incorrect start token\n");
        return;
    }
    
    if( (NMEASentenceBuffer[NMEASentencePtr-1] != 10) || (NMEASentenceBuffer[NMEASentencePtr-2] != 13) ){
        printf("NMEA error: incorrect end token\n");
        return;
    }
    
    if(NMEASentenceBuffer[NMEASentencePtr-5] != '*'){
        printf("NMEA error: incorrect checksum delimiter\n");
        return;
    }
    
    unsigned char checksum = 0;
    unsigned char checksumCounter; 
    for(checksumCounter=1; checksumCounter<(NMEASentencePtr-5); checksumCounter++){
        checksum ^= NMEASentenceBuffer[checksumCounter];
    }
    //printf("Calc checksum: 0x%x\n", checksum);
    char checksumStr[3];
    sprintf(checksumStr, "%02X", checksum);
    if(checksumStr[0] != NMEASentenceBuffer[NMEASentencePtr-4] ||
            checksumStr[1] != NMEASentenceBuffer[NMEASentencePtr-3] ){
        printf("NMEA error: incorrect checksum; calc: %x, rcvd: %c%c\n", 
                checksum, NMEASentenceBuffer[NMEASentencePtr-4], 
                NMEASentenceBuffer[NMEASentencePtr-3]);
        return;
    }
    
    unsigned char sentenceType[4];
    strncpy(sentenceType, NMEASentenceBuffer+3, 3);
    //printf("Got %s\n", sentenceType);
    sentenceType[3] = 0;
    parsePtr = 6;
    
    if(NMEASentenceBuffer[parsePtr] != ','){
        printf("NMEA error: seventh char is not comma\n");
        return;
    }
    if(strcmp(sentenceType, "RMC") == 0){
#ifdef GPS_DEBUG
        printf("Got RMC sentence\n");
        printf("%s", NMEASentenceBuffer);
#endif
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Timestamp: %s\n", parseBuf);
#endif
        //Time is in HHMMSS format
        lastFixInfo.timestamp.hour = dumbAtoI(parseBuf);
        lastFixInfo.timestamp.minute = dumbAtoI(parseBuf+2);
        lastFixInfo.timestamp.second = dumbAtoI(parseBuf+4);
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Validity: %s\n", parseBuf);
#endif
        switch(parseBuf[0]){
            case 'A':
                //Valid data
                gpsGood = TRUE;
                break;
            case 'V':
                //Invalid
                gpsGood = FALSE;
                break;
            default:
                gpsGood = FALSE;
                printf("Invalid validity flag.\n");
                break;
        }
        
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Latitude: %s\n", parseBuf);
#endif
        lastFixInfo.latitude = atof(parseBuf);
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("N/S: %s\n", parseBuf);
#endif
        switch(parseBuf[0]){
            case 'N':
                break;
            case 'S':
                lastFixInfo.latitude = -lastFixInfo.latitude;
            default:
                gpsGood = FALSE;
                printf("Invalid N/S flag.\n");
                break;
        }
        
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Longitude: %s\n", parseBuf);
#endif
        lastFixInfo.longitude = atof(parseBuf);
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("E/W: %s\n", parseBuf);
#endif
        switch(parseBuf[0]){
            case 'E':
                break;
            case 'W':
                lastFixInfo.longitude = -lastFixInfo.longitude;
                break;
            default:
                gpsGood = FALSE;
                printf("Invalid E/W flag.\n");
                break;
        }
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Knots: %s\n", parseBuf);
#endif
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Course: %s\n", parseBuf);
#endif
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Datestamp: %s\n", parseBuf);
#endif
        //Date is in DDMMYY format
        lastFixInfo.timestamp.day = dumbAtoI(parseBuf);
        lastFixInfo.timestamp.month = dumbAtoI(parseBuf+2);
        lastFixInfo.timestamp.year = dumbAtoI(parseBuf+4);
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("Variation: %s\n", parseBuf);
#endif
        
        getNextChars();
#ifdef GPS_DEBUG
        printf("E/W: %s\n", parseBuf);
#endif
        
        return;
    }
    
    if(strcmp(sentenceType, "GSA") == 0){
#ifdef GPS_DEBUG
        printf("Got GSA sentence\n");
        printf("%s", NMEASentenceBuffer);
#endif
        
        //Mode selection
        getNextChars();
        
        //Mode
        getNextChars();
        
        //SV IDs
        unsigned char SVcounter; 
        for(SVcounter=0; SVcounter<12; SVcounter++){
            getNextChars();
        }
        
        //PDOP
        getNextChars();
        
        //HDOP
        getNextChars();
        lastFixInfo.hdop = (uint8_t)(atof(parseBuf)*10);
        
        //VDOP
        getNextChars();
        
        return;
    }
    
    if(strcmp(sentenceType, "GSV") == 0){
#ifdef GPS_DEBUG
        printf("Got GSV sentence\n");
        printf("%s", NMEASentenceBuffer);
#endif
        
        //Message count
        getNextChars();
        
        //Message index
        getNextChars();
        
        //Total SVs
        getNextChars();
        lastFixInfo.nSats = atoi(parseBuf);
        
        return;
    }
}

void sendGPSTelem(void){
    if(gpsGood){
        printf("GPS: %d-%d-%d %d:%d:%d %f %f %d ft HDOP %d %d SVs\n", lastFixInfo.timestamp.year, lastFixInfo.timestamp.month,
                lastFixInfo.timestamp.day, lastFixInfo.timestamp.hour, lastFixInfo.timestamp.minute,
                lastFixInfo.timestamp.second, lastFixInfo.latitude, lastFixInfo.longitude, lastFixInfo.altitude,
                lastFixInfo.hdop, lastFixInfo.nSats);
    }
    else{
        printf("GPS: no fix. %d SVs in view.\n", lastFixInfo.nSats);
    }
    
}

void getNextChars(){
    //Advance past current comma
    parsePtr++;
    
    parseBufPtr = 0;
    while(NMEASentenceBuffer[parsePtr] != ',' && NMEASentenceBuffer[parsePtr] != 0){
        parseBuf[parseBufPtr++] = NMEASentenceBuffer[parsePtr++];
    }
    parseBuf[parseBufPtr] = 0;
}

//Only works for *exactly* two-character strings!
unsigned char dumbAtoI(unsigned char * buf){
    unsigned char retval = 0;
    retval += (buf[0] - 0x30)*10; //tens place
    retval += (buf[1] - 0x30); //ones place
    return retval;
}


void __interrupt(irq(U2RX), low_priority) GPSRxISR(void){
    //unsigned char newChar = U2RXB;
    FIFOPush(&gpsBuffer, U2RXB);
    //printf("G: %d\n", newChar);
}