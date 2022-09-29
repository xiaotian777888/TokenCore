#ifndef __BTXON_PLATFORM_H__
#define __BTXON_PLATFORM_H__

#if defined(_WIN32) || defined(WIN32)
#   define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#   define BX_OS_WIN32
#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500     // 0x0500 = Windows2000
#   endif
#   define _CRT_RAND_S		// for rand_s().
#   include <windows.h>
#   include <tchar.h>
#   define _CSTDIO_
#   define _CSTRING_
#   define _CWCHAR_
#   include <strsafe.h>



#elif defined(__linux__)
#   define BX_OS_LINUX
#   define BX_OS_UNIX_LIKE



#elif defined(__APPLE__)
#   define BX_OS_MACOS
#   define BX_OS_UNIX_LIKE



#else
#	error unsupported platform.
#endif



#ifdef BX_OS_UNIX_LIKE
#   include <sys/types.h>
#   ifdef BX_USE_EMULATOR
#       include <sys/socket.h>
#       include <arpa/inet.h>
#       include <netinet/in.h>
#       ifndef SOCKET
#           define SOCKET int
#       endif
#   endif
#   include <string.h>
#   include <cstdlib>
#   include <stdio.h>
#   include <stdarg.h>
#   include <unistd.h>
//#   ifndef ULONG
//#       define ULONG unsigned long
//#       define DWORD unsigned long
//#       define UINT unsigned int
//#       define UINT8 unsigned char
//#       define USHORT unsigned short
//#       define BYTE unsigned char
//#   endif
//#define CP_UTF8 0
#   define _T(x)  x
#   define TCHAR char
#   define MAX_PATH	1024
int sprintf_s (char* __s, size_t __s_size, const char* __format, ...);
void rand_s(unsigned int* piRand);
//char* strupr_s(char* __s, size_t __s_size);
//int fopen_s(FILE** f, const char* fname, const char* mode);
#endif // BX_OS_UNIX_LIKE

#ifndef u8
#   define u8 unsigned char
#   define u16 unsigned short
#   define u32 unsigned long
#endif

#ifndef ssize_t
#   define ssize_t long
#endif

#include <vector>
typedef std::vector<unsigned char> BINARY;

#endif // __BTXON_PLATFORM_H__
