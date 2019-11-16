/*
 * @Descripttion: 
 * @version: 
 * @Author: 
 * @Date: 2019-10-11 21:08:28
 * @LastEditors: 
 * @LastEditTime: 2019-10-11 21:11:54
 */

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CZC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CZC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef _CZC_H
#define _CZC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CZC_EXPORTS
#define CZC_API __declspec(dllexport)
#else
#define CZC_API __declspec(dllimport)
#endif

CZC_API int WINAPI InitPort(int portNo);

CZC_API void WINAPI ClosePort();

CZC_API int WINAPI SetLed(unsigned char addr, unsigned short stat);

CZC_API int WINAPI GetButton(unsigned char addr);

CZC_API int WINAPI SetOneReg(unsigned char devAddr, unsigned short regAddr,unsigned short stat);

CZC_API int WINAPI GetOneReg(unsigned char addr,unsigned short regAddr);

#ifdef __cplusplus
}
#endif

#endif