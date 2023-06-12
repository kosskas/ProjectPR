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
#define RETURN_CURSOR "\033[0;0H"

#define BLACK      "\033[0;30m"
#define DARK_GRAY  "\033[1;30m"
#define RED        "\033[0;31m"
#define BRIGHT_RED "\033[1;31m"
#define GREEN      "\033[0;32m"
#define BRIGHT_GREEN "\033[1;32m"
#define YELLOW     "\033[0;33m"
#define BRIGHT_YELLOW "\033[1;33m"
#define BLUE       "\033[0;34m"
#define BRIGHT_BLUE "\033[1;34m"
#define MAGENTA    "\033[0;35m"
#define BRIGHT_MAGENTA "\033[1;35m"
#define CYAN       "\033[0;36m"
#define BRIGHT_CYAN "\033[1;36m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE      "\033[1;37m"

#define RESET      "\033[0m"
#define BOLD       "\033[1m"
#define UNDERLINE  "\033[4m"
#define HIGHLIGHT  "\033[7m"

#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_LIGHT_GRAY "\033[47m"
#define BG_DARK_GRAY "\033[100m"
#define BG_LIGHT_RED "\033[101m"
#define BG_LIGHT_GREEN "\033[102m"
#define BG_LIGHT_YELLOW "\033[103m"
#define BG_LIGHT_BLUE "\033[104m"
#define BG_LIGHT_MAGENTA "\033[105m"
#define BG_LIGHT_CYAN "\033[106m"
#define BG_WHITE "\033[107m"

#define compareString(a, b) strcmp(a, b) == 0

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#endif // !GENERAL