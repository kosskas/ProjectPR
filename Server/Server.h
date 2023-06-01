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
    SOCKET *ClientSockets[CLIENT_MAX_NUM] = { NULL };
    addrinfo* result = NULL;
    addrinfo hints;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    HANDLE Clients[CLIENT_MAX_NUM] = { 0 };
    HANDLE SenderTh;
    LPDWORD senderThID;
    LPDWORD ClientsThId[CLIENT_MAX_NUM];
    int online = 0;

    int initWinsock();
    friend DWORD WINAPI ClientListener(LPVOID param);
    friend DWORD WINAPI ClientSender(LPVOID param);
public:
    /**
     * @brief Konstruktor inicjalizujacy dzialanie calego serwera, wywoluje funkcje inicjalizujace WinSock
    */
    Server();
    /**
    * @brief Czysci poprawnie klase
    */
    ~Server();
    /**
     * @brief Uruchamia watki serwera
    */
    void start();
};

/**
 * @brief Sluchacz zdarzen pochodzacych od pojedynczego klienta, dzialajacy jako osobny watek
 * @param param - SOCKET klienta
 * @return 0 jesli watek zakonczyl sie dobrze, 1 jesli zle
*/
DWORD WINAPI ClientListener(LPVOID param);
/**
 * @brief Watek wysylajacy informacje do klientow
 * @param param - Klasa
 * @return 0 jesli watek zakonczyl sie dobrze, 1 jesli zle
*/
DWORD WINAPI ClientSender(LPVOID param);