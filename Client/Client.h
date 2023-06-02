#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class Client {
private:
    /**
     * @brief Struktura potrzebna od inicjalizacji WinSock
    */
    WSADATA wsaData;
    /**
     * @brief Socket ��cz�cy z serwerem
    */
    SOCKET ConnectSocket = INVALID_SOCKET;
    /**
     * @brief Informacje o adresie
    */
    addrinfo* result = NULL,*ptr = NULL, hints;
    /**
     * @brief Bufor bajt�w odebranych
    */
    char recvbuf[DEFAULT_BUFLEN];
    /**
     * @brief D�ugo�� bufora danych
    */
    int recvbuflen = DEFAULT_BUFLEN;
    /**
     * @brief Uchwyt na w�tek MsgReceiverListener
    */
    HANDLE MsgReceiver;
    /**
     * @brief Wska�nik na ID w�tku MsgReceiverListener
    */
    LPDWORD MsgReceiverID;
    /**
     * @brief Funkcja inicjalizuj�ca kompon�ty potrzebne do komunikacji
     * @return 0 je�li konfiguracja przebieg�a pomy�lnie, 1 jesli nie
    */
    int initWinsock(const char* ipadress);
    friend DWORD __stdcall MsgReceiverListener(LPVOID param);
public:
    /**
     * @brief @brief Konstruktor inicjalizujacy aplikacje kliencka
    */
    Client(const char* ipadress);
    /**
     * @brief Przesyla ciag bajtow o podanej dlugosci do serwera
     * @param sendbuf  - przesylany ciag bajtow
     * @param len - dlugosc ciagu liczona w bajtach
     * @return 0 jesli poprawnie przeslano wiadomosc, 1 jesli zle
    */
    int sendMessage(const char* sendbuf, int len);
    /**
    * @brief Czysci poprawnie klase
    */
    ~Client();
};
/**
 * @brief Watek nasluchujacy wiadomosci z serwera
 * @param param Klasa Client
 * @return0 jesli watek zakonczyl sie dobrze, 1 jesli zle
*/
DWORD __stdcall MsgReceiverListener(LPVOID param);
