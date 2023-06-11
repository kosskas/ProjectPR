/*
* TODO
* poprawić kolorowanie konsoli
* X I Y na odwrót!!!!!
* Zakończenie poprawne wątków (PINGER!!!!)
* W przypadku rozłączenia gracza, gracz przegrywa, a jego wąż znika z planszy. DONE?
* Jeśli klient będzie próbował podłączyć się w trakcie gry, zostanie powiadomiony,
że nie może się połączyć. 
* Zrobić plik konfiguracyjny i Sparametryzować Spleepy 

*/


#ifndef _SERVER_H
#define _SERVER_H

struct Player;
class Game;

#include "General.h"
#include "Player.h"
#include "Game.h"



#define compareString(a, b) strcmp(a, b) == 0

/**
 * @brief Tryby wiadomości jakie będzie wysyłał Server
*/
enum MSGMODE {
    CONN = 0xA,
    DISC,
    END,
    SPECTATE,
    MAP
};

/**
 * @brief Pozwala ustawić serwer według podanych parametrów
*/
struct ServerSetup {

    /**
     * @brief Maksymalna długość kolejki oczekujących połączeń
    */
    int backlog;

    /**
     * @brief Określa rozmiar bufora przesyłu danych
    */
    size_t bufferSize;

    /**
     * @brief Określa maksymalną liczbę klientów
    */
    size_t maxNumberOfClients;

    /**
     * @brief Określa numer portu
    */
    int port;

    /**
     * @brief Liczba wierszy
     * 
    */
    unsigned int mapSizeY;

    /**
     * @brief Liczba kolumn
    */
    unsigned int mapSizeX;
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
     * @brief Uchwyt na wątek Broadcast
    */
    HANDLE _SenderTh;

    /**
     * @brief Uchwyt na wątek Pinger
    */
    HANDLE _PingerTh;

    /**
     * @brief Uchwyt na wątek Joiner
    */
    HANDLE _JoinerTh;

    /**
     * @brief Wskaźnik na ID wątku Broadcast
    */
    LPDWORD _senderThID;

    /**
     * @brief Wskaźnik na ID wątku Joiner
    */
    LPDWORD _joinerThID;

    /**
     * @brief Wskaźnik na ID wątku Pinger
    */
    LPDWORD _pingerThID;

    /**
     * @brief Struktura przechowująca informacje ustawienia serwera
    */
    ServerSetup _setup;

    /**
     * @brief flaga informująca wątki o konieczności zakończenia się
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
    friend DWORD __stdcall Broadcast(LPVOID param);
    friend DWORD __stdcall Pinger(LPVOID param);

    //friend struct Player;
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
     * @brief [ + NadajID() ] Używając złożonych funkcji hashujących nadaje Graczowi unikalne id.
     * @return id
    */
    unsigned int setPlayerID();

    /**
     * @brief Funkcja, która w trybie aktywnego oczekiwania czeka na dołączenie nowych klientów, wyłącza się gdy do serwera podłączy się odpowiednia ilość graczy
    */
    void waitForPlayers();

    /**
     * @brief [ + UsuńGracza() ] Zamyka socket Gracza. Zamyka uchwyt do wątku Gracza. Usuwa Gracza.
     * @param Player - obiekt Gracza do usunięcia
    */
    void deletePlayer(Player* player);

    /**
     * @brief [ + ZakończPołączenie() ] Wysyła sygnał do graczy o tym, że połączenie zostanie zamknięte
    */
    void endConnection();

    /**
     * @brief Uruchamia wątek o nazwie Pinger
    */
    void initGarbageCollector();

    /**
     * @brief Uruchamia wątek o nazwie Broadcast
    */
    void initMapBroadcast();

    /**
     * @brief Rozsyłają mapę do klientów temp, to samo co broadcast ale nie wątek
    */
    void sendMap();
public:

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
     * @brief Wymusza zakończenie się wątków. Usuwa obiekty klasy Player z listy Server::Players. Zamyka socker serwera.
    */
    ~Server();
};


/**
 * @brief Sluchacz zdarzeń pochodzących od pojedynczego klienta, działajacy jako osobny wątek
 * @param param - SOCKET klienta
 * @return 0 jeśli wątek zakończył się dobrze, 1 jeśli źle
*/
DWORD __stdcall ClientListener(LPVOID param);


/**
 * @brief Watek rozsyłający informacje do klientów
 * @param param - Wskaźnik na klasę Server
 * @return 0 jeśli wątek zakończył się dobrze
*/
DWORD __stdcall Broadcast(LPVOID param);


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
void codeMessage(Player* player, char* msg, MSGMODE mode);
#endif // !_SERVER_H