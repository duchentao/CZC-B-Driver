#include "stdafx.h"
#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <iostream>
#include "SerialPort.h"

CSerialPort::CSerialPort(void)
{
	m_hComm = NULL;
	memset(&m_CommTimeouts, 0, sizeof(m_CommTimeouts));
	memset(&m_dcb, 0, sizeof(m_dcb));
}

CSerialPort::~CSerialPort(void)
{
	ClosePort();
}

void CSerialPort::ClosePort()
{
	if( m_hComm != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::OpenPort(UINT portNo)
{
    char szPort[20] = {0,};
	
	if (portNo >= 10)
	{
		// Compatible with more than 10 serial ports
		sprintf(szPort, "\\\\.\\COM%d", portNo);
	}
	else
	{
		sprintf(szPort, "COM%d", portNo);
	}

	m_hComm = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);    

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}


bool CSerialPort::InitPort(UINT portNo, UINT baud, char parity, UINT databits, UINT stopsbits)
{
	char szBaud[60] = {0,};
	
	ClosePort();

	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);
	
	if (!OpenPort(portNo))
	{
		return false;
	}
	
	m_CommTimeouts.ReadIntervalTimeout 		   = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier  = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant    = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant   = 1000;
	
	if (!SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{	 
		return false;
	}
	if (!SetupComm(m_hComm, 1024, 1024))
	{
		return false;
	}
	if (!GetCommState(m_hComm, &m_dcb))
	{
	 	return false;
    }
	if (!BuildCommDCB(szBaud, &m_dcb))
	{
		return false;
	}
	if (!SetCommState(m_hComm, &m_dcb))
	{
		return false;
	}

	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	return true;
}

bool CSerialPort::InitPort(UINT portNo, UINT baud, char parity, UINT databits, UINT stopsbits, UINT comTxRxBufSize)
{
	char szBaud[60] = {0,};
	
	ClosePort();
	
	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);
	
	if (!OpenPort(portNo))
	{
		return false;
	}

	m_CommTimeouts.ReadIntervalTimeout 		   = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier  = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant    = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant   = 1000;
	
	if (comTxRxBufSize > 2048)
	{
		return false;
	}
	if (!SetCommTimeouts(m_hComm, &m_CommTimeouts))
	{	 
		return false;
	}
	if (!SetupComm(m_hComm,comTxRxBufSize,comTxRxBufSize))
	{
		return false;
	}
	if (!GetCommState(m_hComm, &m_dcb))
	{
	 	return false;
    }
	if (!BuildCommDCB(szBaud, &m_dcb))
	{
		return false;
	}
	if (!SetCommState(m_hComm, &m_dcb))
	{
		return false;
	}

	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	return true;
}

bool CSerialPort::WriteToPort(char *ptrBuf, int length)
{
	BOOL bResult = true;
	DWORD BytesSent = 0;

	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	
	bResult = WriteFile(m_hComm, ptrBuf, length, &BytesSent, NULL);
	
	if (!bResult)  
	{
		DWORD dwError = GetLastError();
		switch (dwError)
		{
			case ERROR_IO_PENDING:
				PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
				return false;
			default:
				return false;
		}
	}
	
	if (BytesSent != (DWORD)length)
	{
		return false;
	}
	
	return true;
}


bool CSerialPort::ReadForPort(char *ptrBuf, int length)
{
	BOOL  bResult   = true;
	DWORD dwError   = 0;
	DWORD BytesRead = 0;

	bResult = ReadFile(m_hComm, ptrBuf, length, &BytesRead, NULL);
	
	if (!bResult)  
	{ 
		switch (dwError = GetLastError()) 
		{ 
			case ERROR_IO_PENDING:
				PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
				return false;
			default:
				return false;
		}
	}
	
	if (BytesRead != (DWORD)length)
	{
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);
		return false;
	}
			
	return true;
}

bool CSerialPort::ReadForPort(char *ptrBuf, int *readBackLen)
{
	BOOL  bResult   = true;
	DWORD dwError   = 0;
	DWORD BytesRead = 0;
	
	bResult = ReadFile(m_hComm, ptrBuf, 1024, &BytesRead, NULL);
	
	if (!bResult)  
	{ 
		switch (dwError = GetLastError()) 
		{ 
		case ERROR_IO_PENDING:
			PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
			return false;
		default:
			return false;
		}
	}

	*readBackLen = BytesRead;
	
	return true;
}