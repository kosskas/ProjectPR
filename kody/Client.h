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
     * @brief @brief Konstruktor inicjalizuj�cy aplikacj� klienck�
    */
    Client(const char* ipadress);
    /**
     * @brief Przesy�a ci�g bajt�w o podanej d�ugo�ci do serwera
     * @param sendbuf  - przesy�any ci�g bajt�w
     * @param len - d�ugo�� ci�gu liczona w bajtach
     * @return 0 je�li poprawnie przes�ano wiadomo��, 1 je�li �le
    */
    int sendMessage(const char* sendbuf, int len);
    void temp();
    /**
    * @brief Czy�ci poprawnie klas�
    */
    ~Client();
};
/**
 * @brief W�tek nas�uchuj�cy wiadomo�ci z serwera
 * @param param 0
 * @return -
*/
DWORD WINAPI MsgReceiverListener(LPVOID param);
