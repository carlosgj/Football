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
    unsigned char i; 
    for(i=1; i<(NMEASentencePtr-5); i++){
        checksum ^= NMEASentenceBuffer[i];
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
        printf("Got RMC sentence\n");
        printf("%s", NMEASentenceBuffer);
        
        getNextChars();
        printf("Timestamp: %s\n", parseBuf);
        
        getNextChars();
        printf("Validity: %s\n", parseBuf);
        
        getNextChars();
        printf("Latitude: %s\n", parseBuf);
        
        getNextChars();
        printf("N/S: %s\n", parseBuf);
        
        getNextChars();
        printf("Longitude: %s\n", parseBuf);
        
        getNextChars();
        printf("E/W: %s\n", parseBuf);
        
        getNextChars();
        printf("Knots: %s\n", parseBuf);
        
        getNextChars();
        printf("Course: %s\n", parseBuf);
        
        getNextChars();
        printf("Datestamp: %s\n", parseBuf);
        
        getNextChars();
        printf("Variation: %s\n", parseBuf);
        
        getNextChars();
        printf("E/W: %s\n", parseBuf);
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


void __interrupt(irq(U2RX), low_priority) GPSRxISR(void){
    //unsigned char newChar = U2RXB;
    FIFOPush(&gpsBuffer, U2RXB);
    //printf("G: %d\n", newChar);
}