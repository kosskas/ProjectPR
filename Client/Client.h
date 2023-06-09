#ifndef _CLIENT_H
#define _CLIENT_H


#include "General.h"


/**
 * @brief Pozwala ustawić klienta według podanych parametrów
*/
struct ClientSetup {

    /**
     * @brief Numer ip serwera
    */
    const char* serverIP;

    /**
     * @brief Numer portu serwera
    */
    const char* serverPort;

    /**
     * @brief Określa rozmiar bufora odbioru/przesyłu danych
    */
    size_t bufferSize;

    /**
     * @brief Liczba wierszy
    */
    unsigned int mapSizeY;

    /**
     * @brief Liczba kolumn
    */
    unsigned int mapSizeX;
};


/**
 * @brief Klasa Client zarządza grą oraz połączeniem po stronie klienta
*/
class Client {
private:

    enum MSGMODE {
        CONN = 0xA,
        DISC,
        END,
        SPECTATE,
        MAP
    };

    /**
     * @brief Socket łączący z serwerem
    */
    SOCKET _socket = INVALID_SOCKET;

    /**
     * @brief Struktura przechowująca informacje ustawienia klienta
    */
    ClientSetup _setup;

    /**
     * @brief Bufor bajtów odebranych
    */
    char _recvbuf[DEFAULT_BUFLEN] = { 0 };

    /**
     * @brief Długość bufora danych
    */
    int _recvbuflen = DEFAULT_BUFLEN;

    /**
     * @brief Uchwyt na wątek MsgReceiverListener
    */
    HANDLE _MsgReceiverTh;

    /**
     * @brief Uchwyt na wątek MsgSender
    */
    HANDLE _MsgSenderTh;

    /**
     * @brief Uchwyt na wątek KeyEventListener
    */
    HANDLE _KeyEventListenerTh;

    /**
     * @brief Wskaźnik na ID wątku MsgReceiverListener
    */
    LPDWORD _MsgReceiverThID;

    /**
     * @brief Wskaźnik na ID wątku MsgSender
    */
    LPDWORD _MsgSenderThID;

    /**
     * @brief Wskaźnik na ID wątku KeyEventListener
    */
    LPDWORD _KeyEventListenerThID;

    /**
     * @brief Uchwyt standartowe wejście
    */
    HANDLE _hStdin;

    /**
     * @brief Uchwyt standartowe wyjście
    */
    HANDLE _hStdout;

    /**
     * @brief Stare ustawienia konsoli, przywracane na końcu działania aplikacji
    */
    DWORD _fdwSaveOldInMode = 0;

    /**
     * @brief Nowe ustawienia konsoli na czas działania aplikacji
    */
    DWORD _fdwInMode;

    /**
    * @brief Stare ustawienia konsoli, przywracane na końcu działania aplikacji
    */
    DWORD _fdwSaveOldOutMode = 0;

    /**
     * @brief Nowe ustawienia konsoli na czas działania aplikacji
    */
    DWORD _fdwOutMode;

    /**
     * @brief Sczytany aktualny input z klawiatury
    */
    unsigned char _keyInput = VK_LEFT;

    /**
     * @brief Flaga określająca czy trwa gra
    */
    bool _isRunning;
    /**
     * @brief Flaga określająca czy Client jest podłączony do serwera
    */
    bool _isConnected;

    /**
     * @brief Liczba wierszy
    */
    unsigned int _mapSizeY;
    /**
     * @brief Liczba kolumn
    */
    unsigned int _mapSizeX = 0xFFFFFFFF;
    /**
     * @brief Identyfikator gracza
    */
    unsigned int _playerID;
    /**
     * @brief Wynik gracza
    */
    unsigned int _playerScore;


protected:
    
    /**
     * @brief Inicjalizacja WinSock API
     * @return true jeśli inicjalizacja zakończyła się powodzeniem, false jeśli źle
    */
    bool startUpWinsock();
    /**
     * @brief Tworzy socket serwera o paramterach: IPv4, połączeniowe, TCP, ....
     * @return true jeśli socket utworzony pomyślnie, false jeśli źle
    */
    bool createSocket();
    /**
     * @brief Łączy z serwerem
     * @return true jeśli połączenie zakończyło się powodzeniem, false jeśli źle
    */
    bool connectToServer();
    /**
     * @brief Rozłącza z serwerem
     * @return true jeśli rozłączenie zakończyło się powodzeniem, false jeśli źle
    */
    bool disconnectFromServer();
    /**
     * @brief Ucisza socket - nie może ani wysyłać, ani odbierać danych
     * @param sock - SOCKET do uciszenia
     * @return true jeśli pomyślnie uciszono socket, false jeśli nie
    */
    bool shutdownSocket();
    /**
    * @brief Zamyka socket
     * @param sock - SOCKET do zamknięcia
     * @return true jeśli pomyślnie zamknięto socket, false jeśli nie
    */
    bool closeSocket();

    /**
     * @brief Incjalizacja do odczytu klawiatury
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int initConsole();

    /**
     * @brief Incjalizacja wątku do odbierania danych z serwera
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    bool initReceiver();
    /**
     * @brief Incjalizacja wątku do odbierania danych z klawiatury
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    bool initKeyEventListener();
    /**
     * @brief Incjalizacja wątku do wysyłania danych do serwera
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    bool initSender();

    /**
     * @brief Przesyła ciąg bajtów o podanej dlugości do serwera
     * @param sendbuf  - przesyłany ciąg bajtów
     * @param len - dlugość ciągu liczona w bajtach
     * @return 0 jeśli poprawnie przesłano wiadomość, 1 jeśli źle
    */
    bool sendMessage(const char* sendbuf, int len);

    /**
     * @brief Wyświetla mapę, którą wysłał Server oraz status
     * @param map wskaźnik na mapę
    */
    void printGame(const char* map);

    /**
     * @brief Fukcja zwracająca strzałkę w kierunku jaki posiada aktualnie Client
     * @param direction Kierunek jaki posiada aktualnie Client
     * @return Symbol strzałki
    */
    char getArrow(char direction);

    /**
     * @brief Dekoduje wiadomość od Serwera i ją przetwarza
    */
    void decodeMessage();

    friend DWORD __stdcall MsgReceiverListener(LPVOID param);
    friend DWORD __stdcall MsgSender(LPVOID param);
    friend DWORD __stdcall KeyEventListener(LPVOID param);

public:

    /**
     * @brief @brief Konstruktor inicjalizujący aplikacje kliencką
    */
    Client(ClientSetup setup);

    /**
     * @brief Uruchamia wątki oraz grę
    */
    void run();

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
 * @brief Czemu wątek??,
 * @param param 
 * @return 
*/
DWORD __stdcall MsgSender(LPVOID param);


/**
 * @brief Wątek nasłuchujący zdarzeń z klawiatury
 * @param param Klasa Client
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall KeyEventListener(LPVOID param);


#endif // !_CLIENT_H