#ifndef _CLIENT_H
#define _CLIENT_H


#include "General.h"


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
     * @brief Bufor bajtów odebranych
    */
    char recvbuf[DEFAULT_BUFLEN] = { 0 };

    /**
     * @brief Długość bufora danych
    */
    int recvbuflen = DEFAULT_BUFLEN;


    int serverMsg;

    /**
     * @brief Uchwyt na wątek MsgReceiverListener
    */
    HANDLE MsgReceiverTh;

    /**
     * @brief Uchwyt na wątek MsgSender
    */
    HANDLE MsgSenderTh;

    /**
     * @brief Uchwyt na wątek KeyEventListener
    */
    HANDLE KeyEventListenerTh;

    /**
     * @brief Wskaźnik na ID wątku MsgReceiverListener
    */
    LPDWORD MsgReceiverThID;

    /**
     * @brief Wskaźnik na ID wątku MsgSender
    */
    LPDWORD MsgSenderThID;

    /**
     * @brief Wskaźnik na ID wątku KeyEventListener
    */
    LPDWORD KeyEventListenerThID;

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

    /**
     * @brief Sczytany aktualny input z klawiatury
    */
    unsigned char keyInput = VK_LEFT;

    bool _isRunning;

    bool _isConnected;

    unsigned int mapSizeY = 40;
    unsigned int mapSizeX = 63;

    unsigned int playerID;
    unsigned int playerScore;


protected:

    bool startUpWinsock();
    bool createSocket();
    bool connectToServer(const char* ipadress);
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
    */
    void printGame();

    /**
     * @brief Fukcja zwracająca strzałkę w kierunku jaki posiada aktualnie Client
     * @param direction Kierunek jaki posiada aktualnie Client
     * @return Symbol strzałki
    */
    char getArrow(char direction);

    void decodeMessage(uint16_t* msg);

    friend DWORD __stdcall MsgReceiverListener(LPVOID param);
    friend DWORD __stdcall MsgSender(LPVOID param);
    friend DWORD __stdcall KeyEventListener(LPVOID param);

public:

    /**
     * @brief @brief Konstruktor inicjalizujący aplikacje kliencką
    */
    Client(const char* ipadress);

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