#include "Game.h"

Game::Game(list<Player*>& players, int y, int x)
	: sizeX(y), sizeY(x), players(players) {
	gameState = true;
	gameMap = createMap(y,x);
	///Inicjalizuj pozycje graczy oraz długości ich wężów

}

bool Game::checkGameState() {
	return gameState;
}

char** Game::createMap(int Y, int X) {
	char** temp = new char* [Y];
	for (int i = 0; i < Y; i++) {
		temp[i] = new char[X];
		for (int j = 0; j < sizeX; j++) {
			temp[i][j] = '#';
		}
	}
	return temp;
}

void Game::clearMap(char** map) {
	for (int i = 0; i < sizeY; i++) {
		delete[] map[i];
	}
	delete[] map;
	map = nullptr;
}

void Game::placeBonuses(int num) {
	//losuj współrzędne tak długo jak na nich nic nie ma
}

void Game::movePlayer(Player* player) {
	//pobierz obecny kierunek wskazywany przez gracza
	//Przesuń jego postać (dodaj na początek listy nowe kordy, a ogon zwiń) i Rozwiąż konfilikty
	// 
	// Nanieś węża na mapę??
	//Aktualizuj głowę i ogon { begin(), back() }

}

void Game::printSnake(Player* player) {

	for (Point p : player->sprite) {
		int y = p.posY, x = p.posX;
		char player_symbol = (char)(player->ID);
		gameMap[y][x] = player_symbol;
	}

	//Ogon na pusto //Problem z wydłużaniem węży???
	Point last = player->sprite.back();

	gameMap[last.posY][last.posX];
}

Player* Game::getPlayerById(char ID) {
	int pID = (int)ID;
	list<Player*>::iterator it = players.begin();
	advance(it, pID);
	return *it;
}

void Game::getMap(char* msg) {
	int i = 0;
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			msg[i++] = gameMap[y][x];
		}
	}
}

Game::~Game() {
	clearMap(gameMap);
}
