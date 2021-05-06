#ifndef SAT_H
#define	SAT_H

#include <xc.h> 
#include "common.h"

#define SAT_RX_BUF_LEN (256)

unsigned char satRxBuf[SAT_RX_BUF_LEN];

void satInit(void);

#endif

