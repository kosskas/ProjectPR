#pragma once
#include "General.h"
#include "Player.h"
#include "Game.h"

/*
BROADCAST POTENCJALNIE OUT
*/

/**
 * @brief Pozwala na ustawić serwer według podanych parametrów
*/
struct ServerSetup {
    /**
     * @brief ???
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
    list<Player*> Players;

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
     * @brief Struktura przechowująca informacje ustawienia serwera
    */
    ServerSetup _setup;

    /**
     * @brief flaga informująca wątki o konieczności zakończenia się
    */
    bool _isServerRunning;

    /**
     * @brief Instancja gry
    */
    Game game;

    //Funckje watkow
    friend DWORD __stdcall ClientListener(LPVOID param);
    friend DWORD __stdcall Broadcast(LPVOID param);
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



public:

    /**
     * @brief Inicjalizuje WSA. Tworzy socket serwera. Uruchamia serwer pozostawiając go w stanie nasłuchiwania połączeń.
     * @param setup - struktura zawierająca ustawienia, z którymi serwer ma zostać uruchomiony.
    */
    Server(ServerSetup setup);

    /**
     * @brief Wymusza zakończenie się wątków. Usuwa obiekty klasy Player z listy Server::Players. Zamyka socker serwera.
    */
    ~Server();

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
    void deletePlayer(Player* Player);

    /**
     * @brief [ + ZakończPołączenie() ] ???.
    */
    void endConnection(); // TODO


    /**
     * @brief Uruchamia wątek o nazwie Pinger
    */
    void garbageCollector();

    /**
     * @brief !!! ta metoda ma być w klasie Gra !!! Uruchamia wątek o nazwie Broadcast
    */
    void sendMap();

    /**
     * @brief Uruchamia wątki oraz grę
    */
    void run();
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