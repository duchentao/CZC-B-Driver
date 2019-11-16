/*
 * @Descripttion: 
 * @version: 
 * @Author: 
 * @Date: 2019-10-11 21:08:44
 * @LastEditors: 
 * @LastEditTime: 2019-11-11 13:56:37
 */
// czc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <iostream>
#include "SerialPort.h"
#include "crc16.h"
#include "czc.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


CSerialPort port;

CZC_API int WINAPI InitPort(int portNo)
{
	if(!port.InitPort(portNo, 9600, 'N', 8, 1))
	{
		return -1;
	}
	return 1;
}

CZC_API void WINAPI ClosePort()
{
	port.ClosePort();
}

CZC_API int WINAPI SetLed(unsigned char devAddr, unsigned short stat)
{
	return SetOneReg(devAddr, 0x0001,stat);
}

CZC_API int WINAPI GetButton(unsigned char addr)
{
	return GetOneReg(addr,0x0000);
}


CZC_API int WINAPI SetOneReg(unsigned char devAddr, unsigned short regAddr,unsigned short stat)
{
	char sendbuf[16] = { 0, };
	char rcvbuf[16] = { 0, };

	sendbuf[0] = (char)devAddr;
	sendbuf[1] = 0x06;
	sendbuf[2] = (char)(unsigned char)(regAddr >>8);
	sendbuf[3] = (char)(unsigned char)(regAddr);
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


CZC_API int WINAPI GetOneReg(unsigned char addr,unsigned short regAddr)
{
	char sendbuf[16] = { 0, };
	char rcvbuf[16] = { 0, };

	sendbuf[0] = (char)addr;
	sendbuf[1] = 0x03;
	sendbuf[2] = (char)(unsigned char)(regAddr >>8);
	sendbuf[3] = (char)(unsigned char)(regAddr);
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
	return (((unsigned char)rcvbuf[3] << 8) | (unsigned char)rcvbuf[4]);
}