#ifndef GAME
#define GAME


#include "General.h"
#include "Player.h"
struct ServerSetup;
#include "Server.h"


/**
 * @brief Klasa Game zarządza logiką gry
*/
class Game {
private:

	/**
	 * @brief prywatna struktura przechowująca symbole reprezentujące elementy na mapie
	*/
	struct SpritesRepo {

		/**
		 * @brief symbol bonusu na mapie
		*/
		char bonus;

		/**
		 * @brief symbol pustego pola na mapie
		*/
		char empty;

		/**
		 * @brief symbol oznaczający głowę postaci gracza
		*/
		char playerHead;
	};

	unsigned int _placingBonusTries;

	unsigned int _playerStep;

	unsigned int _bonusScoreInc;

	/**
	 * @brief struktura (SpritesRepo) zawierająca symbole wszystkich elementów mapy
	*/
	SpritesRepo _sprites;

	/**
	 * @brief Liczba kolumn
	*/
	const int _sizeX;

	/**
	 * @brief Liczba wierszy
	*/
	const int _sizeY;

	/**
	 * @brief Wskaźnik na mapę o wymiarach Y na X, do mapy odwołuje się poprzez gameMap[y][x]
	*/
	char** _gameMap = NULL;

	/**
	 * @brief Flaga oznaczająca trwanie gry
	*/
	bool _gameState;

	/**
	 * @brief Lista graczy, ta sama jak w klasie Server
	*/
	list<Player*> _players;

	/**
	 * @brief Wynik wymagany do wygrania gry
	*/
	unsigned short _winScore;

	/**
	 * @brief ID gracza, który wygrał
	*/
	int winnerID;
protected:
	/**
	 * @brief Tworzy mape ASCII o wymiarach Y na X
	 * @param Y wiesze
	 * @param X kolumny
	 * @return Wskaźnik do nowo utworzonej mapy
	*/
	char** createMap(int Y, int X);

	/**
	 * @brief Usuwa z pamięci obszar podany jako parametr
	 * @param map Dynamicznie zaalokowany obszar - mapa
	*/
	void clearMap(char** map);

	/**
	 * @brief Nanosi na mapę ASCII postać gracza
	 * @param Lista punktów zajmowanych przez człony węża gracza na mapie
	*/
	void printSnake(Player* player);

	/**
	 * @brief Pobiera kod ASCII gracza
	 * @param player Wskaźnik na konkretnego gracza
	 * @return Symbol ASCII
	*/
	char getPlayerASCII(Player* player);

public:

	/**
	 * @brief Inicjalizuje klasę Game
	 * @param players Lista graczy, ta sama jak w klasie Server
	 * @param setup Wskaźnik to struktury (ServerSetup) potrzbej do ustawienia paarametrów gry
	*/
	Game(list<Player*>& players, ServerSetup* setup);

	/**
	 * @brief Sprawdza stan bieżącej gry
	 * @return Stan gry 
	*/
	bool checkGameState();

	/**
	 * @brief Ustawia losowo na mapę bonusy w ilości podanej przez argument
	 * @param num Liczba bonusów
	*/
	void placeBonuses(int num);

	/**
	 * @brief Usuwa postać nieaktywnego gracza
	 * @param player Wskaźnik na konkretnego gracza
	*/
	void removeSnake(Player* player);

	/**
	 * @brief Wysuje głowę postaci gracza na mapie
	 * @param player Wskaźnik na konkretnego gracza
	*/
	void drawSnakeHead(Player* player);

	/**
	 * @brief Przesuwa postać gracza według jego bieżącego kierunku
	 * @param player Wskaźnik na gracza, którego postać będzie poruszana
	*/
	void movePlayer(Player* player);

	/**
	 * @brief Przekształca mapę do postaci wymaganej przez Server do wysłania
	 * @param msg Wskaźnik na docelową pamięć
	*/
	void getMap(char* msg);

	/**
	 * @brief Zwraca identyfikator gracza, który wygrał rozgrywkę
	 * @return ID gracza, który wygrał
	*/
	int getWinnerID();

	/**
	 * @brief Czyści poprawnie klasę Game
	 * 
	*/
	~Game();
};


#endif // !GAME