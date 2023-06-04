#ifndef GAME
#define GAME
#include "General.h"
#include "Player.h"

/**
 * @brief Klasa Game zarządza logiką gry
*/
class Game {
private:
	bool gameState;
	int sizeX;
	int sizeY;
	char** gameMap;
	//char** createMap();
	//void clearMap();

public:
	Game();
	bool checkGameState();
	void placeBonuses(int num);
	void movePlayer(Player* player);
	~Game();
};
#endif // !GAME
