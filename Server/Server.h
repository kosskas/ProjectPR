#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define CLIENT_MAX_NUM 4
class Server {
private:
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    addrinfo* result = NULL;
    addrinfo hints;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    HANDLE Clients[CLIENT_MAX_NUM];
    LPDWORD ClientsThId[CLIENT_MAX_NUM];
    int online = 0;
    int initWinsock();
    friend DWORD WINAPI ClientListener(LPVOID param);
public:
    /// @brief Initializuje
    Server();
    ~Server();
    /// @brief 
    //void start();
    void start();

};
DWORD WINAPI ClientListener(LPVOID param);
