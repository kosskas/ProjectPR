#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
class Client {
private:
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,*ptr = NULL, hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    HANDLE MsgSender;
    HANDLE MsgReceiver;
    LPDWORD MsgSenderID;
    LPDWORD MsgReceiverID;
    int initWinsock();
    friend DWORD WINAPI MsgReceiverListener(LPVOID param);
public:
    /**
     * @brief @brief Konstruktor inicjalizuj¹cy aplikacjê klienck¹
    */
    Client(const char* ipadress);
    /**
     * @brief Przesy³a ci¹g bajtów o podanej d³ugoœci do serwera
     * @param sendbuf  - przesy³any ci¹g bajtów
     * @param len - d³ugoœæ ci¹gu liczona w bajtach
     * @return 0 jeœli poprawnie przes³ano wiadomoœæ, 1 jeœli Ÿle
    */
    int sendMessage(const char* sendbuf, int len);
    void temp();
    /**
    * @brief Czyœci poprawnie klasê
    */
    ~Client();
};
/**
 * @brief W¹tek nas³uchuj¹cy wiadomoœci z serwera
 * @param param 0
 * @return -
*/
DWORD WINAPI MsgReceiverListener(LPVOID param);
