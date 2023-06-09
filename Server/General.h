#ifndef GENERAL
#define GENERAL


#undef UNICODE
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <stdint.h>

#define LEFT VK_LEFT
#define RIGHT VK_RIGHT
#define UP VK_UP
#define DOWN VK_DOWN

#define BACKLOG_LENGTH SOMAXCONN   // this is to long backlog queue !!!
#define MAX_NUMBER_OF_CLIENTS 2
#define BUFFER_SIZE 4096//512
#define PORT 27015

#define MAP_SIZE_X 40 
#define MAP_SIZE_Y 20


#pragma comment (lib, "Ws2_32.lib")


using std::list;
using std::advance;


#endif // !GENERAL