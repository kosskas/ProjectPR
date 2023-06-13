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
    char serverIP[16];

    /**
     * @brief Numer portu serwera
    */
    char serverPort[6];

};


/**
 * @brief Klasa Client zarządza grą oraz połączeniem po stronie klienta
*/
class Client {
private:
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
    char _recvbuf[RECV_BUFFER_SIZE] = { 0 };

    /**
     * @brief Określa rozmiar bufora odbioru/przesyłu danych
    */
    size_t _recvbuflen = RECV_BUFFER_SIZE;

    /**
     * @brief Uchwyt na wątek MsgReceiverListener
    */
    HANDLE _MsgReceiverTh;

    /**
     * @brief Uchwyt na wątek KeyEventListener
    */
    HANDLE _KeyEventListenerTh;

    /**
     * @brief Wskaźnik na ID wątku MsgReceiverListener
    */
    LPDWORD _MsgReceiverThID;

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
     * @brief Flaga określająca czy Client otrzymał wiadomość początkową od Server
    */
    bool _hasReceivedConnMsg;

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
    unsigned short _playerScore;
    /**
     * @brief Wymagany wynik do wygrania gry przesłany przez Server
    */
    unsigned int _serverWinScore;
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
     * @brief Funkcja zwracająca strzałkę w kierunku jaki posiada aktualnie Client
     * @param direction Kierunek jaki posiada aktualnie Client
     * @return Symbol strzałki
    */
    char getArrow(char direction);

    /**
     * @brief Dekoduje wiadomość od Serwera i ją przetwarza
    */
    void decodeMessage();
    /**
     * @brief Ustawia kolor w zależności od ID gracza
     * @param buf bufor gdzie będzie umieszczony kod koloru
     * @param id Pole z graczem 
    */
    void getColorById(char* buf, char id);

    friend DWORD __stdcall MsgReceiverListener(LPVOID param);;
    friend DWORD __stdcall KeyEventListener(LPVOID param);

public:

    /**
     * @brief Tryby wiadomości jakie będzie odbierał Client od Server
    */
    enum MSGMODE {
        /**
         * @brief Połączono z Server, odbierz informacje o grze
        */
        CONN,
        /**
         * @brief Koniec gry, połączenie zostanie zamknięte
        */
        DISC,
        /**
         * @brief Poza grą. Odbierz aktualną mapę
        */
        SPECTATE,
        /**
         * @brief W grze. Odbierz aktualną mapę
        */
        MAP
    };

    /**
    * @brief Konfiguruje klasę Client według zadanej konfiguracji
    * @param setup Wskaźnik na konfiguracje
    */
    static void setUp(ClientSetup *setup);

    /**
     * @brief @brief Konstruktor inicjalizujący aplikacje kliencką
     * @param setup Wskaźnik na konfiguracje, z którymi Client ma zostać uruchomiony.
    */
    Client(ClientSetup *setup);

    /**
     * @brief Uruchamia wątki oraz grę
    */
    void run();

    /**
    * @brief Czyści poprawnie klasę
    * 
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


#endif // !_CLIENT_H