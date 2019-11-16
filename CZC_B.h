#ifndef _CZC_B_
#define _CZC_B_

#include "stdafx.h"
#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <iostream>
#include "SerialPort.h"
#include "crc16.h"

int InitPort(int portNo);

int SetLed(unsigned char addr, unsigned short stat);

int GetLed(unsigned char addr);

#endif