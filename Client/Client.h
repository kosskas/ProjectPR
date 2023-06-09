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
        PINGER,
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

    bool _isRunning;

    bool _isConnected;

    unsigned int _mapSizeY;
    unsigned int _mapSizeX = 0xFFFFFFFF;

    unsigned int _playerID;
    unsigned int _playerScore;


protected:

    bool startUpWinsock();
    bool createSocket();
    bool connectToServer();
    bool disconnectFromServer();
    bool shutdownSocket();
    bool closeSocket();

    /**
     * @brief Funkcja inicjalizująca komponęty potrzebne do komunikacji
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    //int InitWinsock(const char* ipadress);

    /**
     * @brief Incjalizacja do odczytu klawiatury
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    int initConsole();

    /**
     * @brief Incjalizacja do wątków
     * @return 0 jeśli konfiguracja przebiegła pomyślnie, 1 jesli nie
    */
    bool initReceiver();
    bool initKeyEventListener();
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