/*
///TODO
* Zakończenie poprawne wątków (PINGER!!!!)
* poprawić komunikaty
* sekcja krytyczna
* warunki brzegowe wartości czytanych z plików config
*/
/*
///Zaobserwonane błędy
* Wyłączanie pingera -> crash serwera: Nierozwiązane/(Rozwiązane metodą bomby atomowej)
* Czołowe zderzenie węży -> crash serwera: Rozwiązane
* Dziwny memory wyjątek - pojawia się rzadko po skończeniu działania dektruktora serwera -> crash: Nierozwiązane
*/

#ifndef _SERVER_H
#define _SERVER_H

struct Player;
class Game;
#include "General.h"
#include "Player.h"
#include "Game.h"


/**
 * @brief Pozwala ustawić serwer według podanych parametrów
*/
struct ServerSetup {

    /**
     * @brief Maksymalna długość kolejki oczekujących połączeń
    */
    unsigned int backlog;

    unsigned int bonusScoreInc;

    /**
     * @brief Liczba kolumn
    */
    unsigned int mapSizeX;

    /**
     * @brief Liczba wierszy
    */
    unsigned int mapSizeY;

    unsigned int numberOfBonusesAtOnce;

    /**
     * @brief Określa maksymalną liczbę klientów
    */
    size_t numberOfPlayers;

    unsigned int placingBonusTries;

    unsigned int playerStep;

    /**
     * @brief Określa numer portu
    */
    int port;

    unsigned int sleepMsEndConnection;

    unsigned int sleepMsPinger;

    unsigned int sleepMsRun;

    char spriteBonus;

    char spriteEmpty;

    char spritePlayerHead;

    unsigned int timespanBetweenBonuses;

    /**
     * @brief Wynik wymagany do wygrania gry
    */
    unsigned short winScore;
};


/**
 * @brief Klasa Server zarządza połączeniami oraz grą
*/
class Server {
private:

    /**
     * @brief Socket nasłuchujący nowych połączeń
    */
    SOCKET _socket;

    /**
     * @brief Lista aktualnie połączonych graczy
    */
    list<Player*> _players;

    /**
     * @brief Uchwyt na wątek Pinger
    */
    HANDLE _PingerTh;


    /**
     * @brief Wskaźnik na ID wątku Pinger
    */
    LPDWORD _pingerThID;

    /**
     * @brief Struktura przechowująca informacje ustawienia serwera
    */
    ServerSetup _setup;

    /**
     * @brief Flaga informująca wątki o konieczności zakończenia się
    */
    volatile bool _isServerRunning;
    /**
     * @brief Instancja gry
    */
    Game* _game;
    /**
     * @brief Bufor przechowujący mapę gry w stanie do wysłania
    */
    char* _mapBuffer;

    /**
     * @brief Wielkość wysyłanej mapy
    */
    int _mapMsgSize;

    friend DWORD __stdcall ClientListener(LPVOID param);
    friend DWORD __stdcall Pinger(LPVOID param);
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
     * @brief Powiązuje socket serwera z domyślnym adresem, portem podanym w ustawieniach serwera (ServerSetup), IPv4
     * @return true jeśli udało się powiązać socket serwera, false jeśli źle
    */
    bool bindSocket();

    /**
     * @brief Rozpoczyna nasłuchiwanie połączeń przez socket serwera z backlogiem o długości podanej w ustawieniach serwera (ServerSetup)
     * @return true jeśli serwer rozpoczął nasłuchiwanie, false jeśli źle
    */
    bool startListen();

    /**
     * @brief Ucisza socket - nie może ani wysyłać, ani odbierać danych
     * @param sock - SOCKET do uciszenia
     * @return true jeśli pomyślnie uciszono socket, false jeśli nie
    */
    bool shutdownSocket(SOCKET sock);

    /**
     * @brief Zamyka socket
     * @param sock - SOCKET do zamknięcia
     * @return true jeśli pomyślnie zamknięto socket, false jeśli nie
    */
    bool closeSocket(SOCKET sock);

    /**
     * @brief Używając złożonych funkcji hashujących nadaje Graczowi unikalne id.
     * @return id
    */
    unsigned int setPlayerID();

    /**
     * @brief Funkcja, która w trybie aktywnego oczekiwania czeka na dołączenie nowych klientów, wyłącza się gdy do serwera podłączy się odpowiednia ilość graczy
    */
    void waitForPlayers();

    /**
     * @brief Zamyka socket Gracza. Zamyka uchwyt do wątku Gracza. Usuwa Gracza.
     * @param Player - obiekt Gracza do usunięcia
    */
    void deletePlayer(Player* player);

    /**
     * @brief Wysyła sygnał do graczy o tym, że połączenie zostanie zamknięte
    */
    void endConnection();

    /**
     * @brief Uruchamia wątek o nazwie Pinger
    */
    void initGarbageCollector();

    /**
     * @brief Rozsyłają mapę do klientów temp, to samo co broadcast ale nie wątek
    */
    void sendMessage();
public:
    /**
     * @brief Tryby wiadomości jakie będzie wysyłał Server do Client
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

    static void setUp(ServerSetup* setup);

    /**
     * @brief Inicjalizuje WSA. Tworzy socket serwera. Uruchamia serwer pozostawiając go w stanie nasłuchiwania połączeń.
     * @param setup - struktura zawierająca ustawienia, z którymi serwer ma zostać uruchomiony.
    */
    Server(ServerSetup setup);

    /**
     * @brief Uruchamia wątki oraz grę
    */
    void run();

    /**
     * @brief Pobiera wymiar X
     * @return Liczba kolumn
    */
    unsigned int getXSize();

    /**
     * @brief Pobiera wymiar X
     * @return Liczba wierszy
    */
    unsigned int getYSize();

    /**
     * @brief Zwraca identyfikator gracza, który wygrał rozgrywkę
     * @return ID gracza, który wygrał
    */
    int getWinnerID();

    /**
     * @brief Zwraca wymagany wynik do wygrania gry
     * @return Wynik
    */
    unsigned int getWinCondition();

    /**
     * @brief Wymusza zakończenie się wątków. Usuwa obiekty klasy Player z listy Server::Players. Zamyka socker serwera.
     * 
    */
    ~Server();
};

/**
 * @brief Sluchacz zdarzeń pochodzących od pojedynczego klienta, działajacy jako osobny wątek
 * @param param - Wskaźnik na Player
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall ClientListener(LPVOID param);

/**
 * @brief Watek sprawdzający co określony połączenie z użytkownikiem, jeśli nie ma to usuwa go z gry
 * @param param - Wskaźnik na klasę Server
 * @return 0 jeśli wątek zakończył się dobrze
*/
DWORD __stdcall Pinger(LPVOID param);
/**
 * @brief Koduje wiadomość do gracza
 * @param player Wskaźnik na Player
 * @param msg bufor na wiadomość
 * @param mode tryb wiadomości
*/
void codeMessage(Player* player, char* msg, Server::MSGMODE mode);
#endif // !_SERVER_H