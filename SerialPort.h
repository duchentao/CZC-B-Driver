#ifndef SERIALPORT_H_
#define SERIALPORT_H_
#include <windows.h>

class CSerialPort
{
public:

	CSerialPort(void);
	
	~CSerialPort(void);

public:

	bool OpenPort(UINT portNo);

	void ClosePort();
	
	bool InitPort(UINT portNo, UINT baud, char parity, UINT databits, UINT stopsbits);

	bool InitPort(UINT portNo, UINT baud, char parity, UINT databits, UINT stopsbits, UINT comTxRxBufSize);
	
	bool WriteToPort(char *ptrBuf, int length);
	
	bool ReadForPort(char *ptrBuf, int length);
	
	bool ReadForPort(char *ptrBuf, int *readBackLen);
		
private:

	HANDLE  m_hComm;

	COMMTIMEOUTS m_CommTimeouts;

	DCB m_dcb;
};

#endif

