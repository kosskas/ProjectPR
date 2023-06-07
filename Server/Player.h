#ifndef PLAYER
#define PLAYER


#include "General.h"


/**
 * @brief Zawiera parę określającą położenie XY
*/
struct Point {

    /**
     * @brief Współrzędna X
    */
    int posX;

    /**
     * @brief Współrzędna Y
    */
    int posY;
};


/**
 * @brief Zawiera wszystktie informacje o pojedynczym graczu-kliencie
*/
struct Player {

    /**
     * @brief Uchwyt na wątek ClientListener
    */
    HANDLE thHandle;

    /**
     * @brief Wskaźnik na ID wątku ClientListener
    */
    DWORD thId;

    /**
     * @brief Socket łączący ze stroną kliencką
    */
    SOCKET sock;

    /**
     * @brief Adres IP klienta
    */
    IN_ADDR ip;

    /**
     * @brief Nr portu klienta
    */
    USHORT port;

    /**
     * @brief Flaga określająca czy dany klient istnieje
    */
    bool isRunning;

    /**
     * @brief Określa obecnie obrany kierunek 
    */
    unsigned char currentDirection;

    /**
     * @brief Zdobyty wynik podczas gry
    */
    int score;

    /**
     * @brief Unikalny identyfikator, symbol ASCII, który będzie rysowany na mapie jako postać gracza
    */
    int ID;

    /**
     * @brief Określa położenie wszystkich członów postaci gracza na mapie
    */
    list<Point> sprite;

    /**
     * @brief Określa położenie głowy postaci gracza
    */
    Point head;

    /**
     * @brief Określa położenie ostatniego członu postaci gracza
    */
    Point tail;
};


#endif // !PLAYER