#ifndef GENERAL
#define GENERAL
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEFAULT_BUFLEN 4096//512
#define DEFAULT_PORT "27015"

#define VK_Q 0x51 // ASCII of 'Q'

#define MAP_SIZE_X 40 
#define MAP_SIZE_Y 20


#define compareString(a, b) strcmp(a, b) == 0


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#endif // !GENERAL