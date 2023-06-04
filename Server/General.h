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

#define LEFT VK_LEFT
#define RIGHT VK_RIGHT
#define UP VK_UP
#define DOWN VK_DOWN

#pragma comment (lib, "Ws2_32.lib")

using std::list;
#endif // !GENERAL