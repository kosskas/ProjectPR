#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
using namespace std;


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define CLIENT_MAX_NUM 4
class Server {
private:
    //WinSock
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    addrinfo* result = NULL;
    addrinfo hints;
    list<SOCKET*> ClientSockets;
    //Wątki
    HANDLE SenderTh;
    HANDLE PingerTh;
    LPDWORD senderThID;
    LPDWORD pingerThID;
    //LPDWORD ClientsThId[CLIENT_MAX_NUM];

    int initWinsock();

    //Funckje watkow
    friend DWORD WINAPI ClientListener(LPVOID param);
    friend DWORD WINAPI ClientMessenger(LPVOID param);
    friend DWORD WINAPI Pinger(LPVOID param);
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
 * @brief Watek wysyłający informacje do klientow
 * @param param - Klasa
 * @return 0 jesli watek zakonczyl sie dobrze
*/
DWORD WINAPI ClientMessenger(LPVOID param);

/**
 * @brief Watek sprawdzajacy co okreslony czas czy uzytkownik istnieje, jesli nie to go wypiernicza z gry
 * @param param - Tajemniczy paramentr, ktorego nie bede tu omawiac
 * @return 0 jesli watek zakonczyl sie dobrze
*/
DWORD WINAPI Pinger(LPVOID param);