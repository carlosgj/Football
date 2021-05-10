#ifndef SAT_H
#define	SAT_H

#include <xc.h> 
#include "common.h"
#include "FIFO.h"

#define SAT_TX_BUF_LEN (50)

//unsigned char satRxBuf[SAT_RX_BUF_LEN];
//unsigned char satTxBuf[SAT_TX_BUF_LEN];

FIFO satRxBuf;
unsigned char txInProgress = FALSE;

void satInit(void);
void satSendBuffer(unsigned char *buf, unsigned char len, unsigned char override, unsigned char retry);
void satPeriodic(void);

#endif

