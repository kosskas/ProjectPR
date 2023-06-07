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

/**
 * @brief Klasa Client zarządza grą oraz połączeniem po stronie klienta
*/
class Client {
private:
    /**
     * @brief Socket łączący z serwerem
    */
    SOCKET ConnectSocket = INVALID_SOCKET;
    /**
     * @brief Bufor bajtów odebranych
    */
    char recvbuf[DEFAULT_BUFLEN] = { 0 };
    /**
     * @brief Długość bufora danych
    */
    int recvbuflen = DEFAULT_BUFLEN;
    /**
     * @brief Uchwyt na wątek MsgReceiverListener
    */
    HANDLE MsgReceiver;
    /**
     * @brief Uchwyt na wątek KeyEventListener
    */
    HANDLE keyEventListener;
    /**
     * @brief Wskaźnik na ID wątku MsgReceiverListener
    */
    LPDWORD MsgReceiverID;
    /**
     * @brief Wskaźnik na ID wątku KeyEventListener
    */
    LPDWORD keyEventListenerID;
    /**
     * @brief Uchwyt standartowe wejście
    */
    HANDLE hStdin;
    /**
     * @brief Uchwyt standartowe wyjście
    */
    HANDLE hStdout;
    /**
     * @brief Stare ustawienia konsoli, przywracane na końcu działania aplikacji
    */
    DWORD fdwSaveOldInMode = 0;
    /**
     * @brief Nowe ustawienia konsoli na czas działania aplikacji
    */
    DWORD fdwInMode;
    /**
    * @brief Stare ustawienia konsoli, przywracane na końcu działania aplikacji
    */
    DWORD fdwSaveOldOutMode = 0;
    /**
     * @brief Nowe ustawienia konsoli na czas działania aplikacji
    */
    DWORD fdwOutMode;
    //
    /**
     * @brief Sczytany aktualny input z klawiatury
    */
    unsigned char keyInput = VK_LEFT;
    /**
     * @brief Funkcja inicjalizująca komponęty potrzebne do komunikacji
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int InitWinsock(const char* ipadress);
    /**
     * @brief Incjalizacja do odczytu klawiatury
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int InitConsole();
    /**
     * @brief Incjalizacja do wątków
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int InitThreads();
    /**
     * @brief Przesyła ciąg bajtów o podanej dlugości do serwera
     * @param sendbuf  - przesyłany ciąg bajtów
     * @param len - dlugość ciągu liczona w bajtach
     * @return 0 jeśli poprawnie przesłano wiadomość, 1 jeśli źle
    */
    int sendMessage(const char* sendbuf, int len);
    /**
     * @brief Wyświetla mapę, którą wysłał Server oraz status
    */
    void printGame();
    /**
     * @brief Fukcja zwracająca strzałkę w kierunku jaki posiada aktualnie Client
     * @param direction Kierunek jaki posiada aktualnie Client
     * @return Symbol strzałki
    */
    char getArrow(char direction);

    friend DWORD __stdcall MsgReceiverListener(LPVOID param);
    friend DWORD __stdcall KeyEventListener(LPVOID param);
public:
    /**
     * @brief @brief Konstruktor inicjalizujący aplikacje kliencką
    */
    Client(const char* ipadress);
    /**
     * @brief Startuje działanie aplikacji
    */
    void start();
    /**
    * @brief Czyści poprawnie klasę
    */
    ~Client();
};
/**
 * @brief Watek nasluchujacy wiadomosci z serwera
 * @param param Klasa Client
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall MsgReceiverListener(LPVOID param);
/**
 * @brief Wątek nasłuchujący zdarzeń z klawiatury
 * @param param Klasa Client
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall KeyEventListener(LPVOID param);