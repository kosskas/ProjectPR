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
     * @brief Konstruktor inicjalizuj¹cy dzia³anie ca³ego serwera, wywo³uje funkcjê inicjalizuj¹ce WinSock
    */
    Server();
    /**
    * @brief Czyœci poprawnie klasê
    */
    ~Server();
    /**
     * @brief Uruchamia w¹tki serwera
    */
    void start();

};

/**
 * @brief S³uchacz zdarzeñ pochodz¹cych od pojedynczego klienta, dzia³aj¹cy jako osobny w¹tek
 * @param param - SOCKET klienta
 * @return 0 jeœli w¹tek zakoñczy³ siê dobrze, 1 jeœli Ÿle
*/
DWORD WINAPI ClientListener(LPVOID param);
/**
 * @brief W¹tek wysy³aj¹cy informacje do klientów
 * @param param - Klasa
 * @return 0 jeœli w¹tek zakoñczy³ siê dobrze, 1 jeœli Ÿle
*/
DWORD WINAPI ClientSender(LPVOID param);