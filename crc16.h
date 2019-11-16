#ifndef _CRC_
#define _CRC_

#include "stdafx.h"
#include <stdio.h>
#include <string.h>


unsigned int getCRC16(unsigned char array[], unsigned int length);


unsigned int UART_CRC16_Work(unsigned char *CRC_Buf, unsigned int CRC_Leni);

#endif