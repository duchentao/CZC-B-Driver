#include "stdafx.h"
#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <iostream>
#include "SerialPort.h"
#include "crc16.h"


CSerialPort port;

int InitPort(int portNo)
{
	if(!port.InitPort(portNo, 9600, 'N', 8, 1))
	{
		return -1;
	}
	return 1;
}


int SetLed(unsigned char addr, unsigned short stat)
{
	char sendbuf[16] = { 0, };
	char rcvbuf[16] = { 0, };

	sendbuf[0] = (char)addr;
	sendbuf[1] = 0x06;
	sendbuf[2] = 0x00;
	sendbuf[3] = 0x00;
	sendbuf[4] = (char)(unsigned char)(stat >>8);
	sendbuf[5] = (char)(unsigned char)(stat);

	unsigned short val = UART_CRC16_Work((unsigned char *)sendbuf, 6);

	sendbuf[6] = (char)(unsigned char)(val >> 8);
	sendbuf[7] = (char)(unsigned char)val;

	if(!port.WriteToPort(sendbuf, 8))
	{
		return -1;
	}

	Sleep(200);

	if(!port.ReadForPort(rcvbuf, 8))
	{
		return -1;
	}

	return 1;
}

int GetLed(unsigned char addr)
{
	char sendbuf[16] = { 0, };
	char rcvbuf[16] = { 0, };

	sendbuf[0] = (char)addr;
	sendbuf[1] = 0x03;
	sendbuf[2] = 0x00;
	sendbuf[3] = 0x00;
	sendbuf[4] = 0x00;
	sendbuf[5] = 0x01;

	unsigned short val = UART_CRC16_Work((unsigned char *)sendbuf, 6);

	sendbuf[6] = (char)(unsigned char)(val >> 8);
	sendbuf[7] = (char)(unsigned char)val;
	
	if(!port.WriteToPort(sendbuf, 8))
	{
		return -1;
	}
	Sleep(200);
	if(!port.ReadForPort(rcvbuf, 7))
	{
		
		return -1;
	}

	unsigned short val2 = UART_CRC16_Work((unsigned char *)rcvbuf, 5);

	if (val2 != (((unsigned char)rcvbuf[5] << 8) | (unsigned char)rcvbuf[6]))
	{
		return -1;
	}
	return ((rcvbuf[3] << 8) | rcvbuf[4]);
}
