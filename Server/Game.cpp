#include "Game.h"


// - - - - - - - - - - Game :: private - - - - - - - - - - \\


char** Game::createMap(int Y, int X) 
{
	char** temp = new char*[Y];
	for (int y = 0; y < Y; y++) {
		temp[y] = new char[X];
		for (int x = 0; x < X; x++) {
			temp[y][x] = '8';
		}
	}
	return temp;
}


void Game::clearMap(char** map)
{
	for (int i = 0; i < _sizeY; i++) {
		delete[] map[i];
	}
	delete[] map;
	map = nullptr;
}


void Game::printSnake(Player* player)
{
	for (Point p : player->sprite) {
		int y = p.posY, x = p.posX;
		char player_symbol = (char)(player->ID);
		_gameMap[y][x] = player_symbol;
	}

	//Ogon na pusto //Problem z wydłużaniem węży???
	Point last = player->sprite.back();

	_gameMap[last.posY][last.posX];
}


Player* Game::getPlayerById(char ID) 
{
	int pID = (int)ID;
	list<Player*>::iterator it = _players.begin();
	advance(it, pID);
	return *it;
}


// - - - - - - - - - - Game :: public - - - - - - - - - - \\


Game::Game(list<Player*>& players, int y, int x)
	: _sizeX(x), _sizeY(y), _players(players) 
{
	_gameState = true;
	_gameMap = createMap(y,x);
	///Inicjalizuj pozycje graczy oraz długości ich wężów
	for (Player* player : _players) {
		_gameMap[x / (player->ID+1)][y / (player->ID+1)] = (char)player->ID;
	}

}


bool Game::checkGameState() 
{
	return _gameState;
}


void Game::placeBonuses(int num)
{
	//losuj współrzędne tak długo jak na nich nic nie ma
}


void Game::movePlayer(Player* player) 
{
	//pobierz obecny kierunek wskazywany przez gracza
	//Przesuń jego postać (dodaj na początek listy nowe kordy, a ogon zwiń) i Rozwiąż konfilikty
	// 
	// Nanieś węża na mapę??
	//Aktualizuj głowę i ogon { begin(), back() }

}


void Game::getMap(char* msg) 
{
	int i = 0;
	for (int y = 0; y < _sizeY; y++) {
		for (int x = 0; x < _sizeX; x++) {
			msg[i++] = _gameMap[y][x];
		}
	}
}


Game::~Game() 
{
	clearMap(_gameMap);
}
