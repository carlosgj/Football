#ifndef RS422_H
#define	RS422_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "common.h"
#include "commConstants.h"

void UART_Init(void);
void putch(unsigned char theByte);

inline void UART_StartTx(void);

#endif

