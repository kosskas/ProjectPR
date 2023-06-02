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
    /**
     * @brief Struktura potrzebna od inicjalizacji WinSock
    */
    WSADATA wsaData; 
    /**
     * @brief Socket nasłuchujący nowych połączeń
    */
    SOCKET ListenSocket = INVALID_SOCKET;
    /**
     * @brief Informacje o adresie
    */
    addrinfo* result = NULL, hints;
    /**
     * @brief Lista aktualnie połączonych użytkowników
    */
    list<SOCKET*> ClientSockets;
    //Wątki
    /**
     * @brief Uchwyt na wątek Broadcast
    */
    HANDLE SenderTh;
    /**
     * @brief Uchwyt na wątek Pinger
    */
    HANDLE PingerTh;
    /**
     * @brief Uchwyt na wątek Joiner
    */
    HANDLE JoinerTh;
    /**
     * @brief Wskaźnik na ID wątku Broadcast
    */
    LPDWORD senderThID;
    /**
     * @brief Wskaźnik na ID wątku Joiner
    */
    LPDWORD joinerThID;
    /**
     * @brief Wskaźnik na ID wątku Pinger
    */
    LPDWORD pingerThID;
    /**
     * @brief Funkcja inicjalizująca komponęty potrzebne do komunikacji
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int initWinsock();

    //Funckje watkow
    friend DWORD __stdcall NewClientJoiner(LPVOID param);
    friend DWORD __stdcall ClientListener(LPVOID param);
    friend DWORD __stdcall Broadcast(LPVOID param);
    friend DWORD __stdcall Pinger(LPVOID param);
public:
    /**
     * @brief Konstruktor inicjalizujący działanie calego serwera, wywołuje funkcje inicjalizujące WinSock
    */
    Server();
    /**
    * @brief Czyści poprawnie klasę
    */
    ~Server();
    /**
     * @brief Uruchamia wątki serwera
    */
    void start();
};

/**
 * @brief Sluchacz zdarzeń pochodzących od pojedynczego klienta, działajacy jako osobny wątek
 * @param param - SOCKET klienta
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall ClientListener(LPVOID param);
/**
 * @brief Watek rozsyłający informacje do klientów
 * @param param - Klasa
 * @return 0 jeśli wątek zakończył się dobrze
*/
DWORD __stdcall Broadcast(LPVOID param);

/**
 * @brief Watek sprawdzający co określony czas czy użytkownik istnieje, jeśli nie to go wypiernicza z gry
 * @param param - Tajemniczy paramentr, którego nie będe tu omawiać
 * @return 0 jeśli wątek zakończył się dobrze
*/
DWORD __stdcall Pinger(LPVOID param);
/**
 * @brief Wątek oczekujący na dołączenie nowych klientów, wyłącza się gdy do serwera podłączy się odpowiednia ilość graczy
 * @param param - Tajemniczy paramentr, którego nie będe tu omawiać
 * @return 0 jeśli wątek zakończył się dobrze
*/
DWORD __stdcall NewClientJoiner(LPVOID param);