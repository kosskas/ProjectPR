#include "Game.h"


// - - - - - - - - - - Game :: protected - - - - - - - - - - \\


char** Game::createMap(int Y, int X) 
{
	char** temp = new char*[Y];
	for (int y = 0; y < Y; y++) {
		temp[y] = new char[X];
		for (int x = 0; x < X; x++) {
			temp[y][x] = EMPTY_SPRITE;
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
		_gameMap[y][x] = getPlayerASCII(player);
	}
}


char Game::getPlayerASCII(Player* player)
{
	return (char)(player->ID) + '0';
}


// - - - - - - - - - - Game :: public - - - - - - - - - - \\


Game::Game(list<Player*>& players, ServerSetup* setup) :
	_sizeX(setup->mapSizeX),
	_sizeY(setup->mapSizeY),
	_players(players),
	_placingBonusTries(setup->placingBonusTries),
	_playerStep(setup->playerStep),
	_bonusScoreInc(setup->bonusScoreInc),
	_winScore(setup->winScore)
{
	_sprites.bonus = setup->spriteBonus;
	_sprites.empty = setup->spriteEmpty;
	_sprites.playerHead = setup->spritePlayerHead;

	_gameState = true;
	_gameMap = createMap(_sizeY, _sizeX);

	int sty = 2, stx = 2;
	///Inicjalizuj pozycje graczy oraz długości ich wężów
	for (Player* player : _players) {
		player->sprite.push_front({ sty += 4, stx += 2 });
		player->sprite.push_front({ sty + 1, stx });
	}
}


bool Game::checkGameState() 
{
	int n_of_playing = 0;
	int winner = 0;
	for (Player* player : _players) {
		if (player->isPlaying) {
			winner = player->ID;
			n_of_playing++;
		}
	}
	if (n_of_playing == 1) {
		_gameState = false;
		winnerID = winner;
	}
	return _gameState;
}


void Game::placeBonuses(int num)
{
	// Inicjalizacja generatora liczb losowych
	random_device rd;
	mt19937 generator(rd());

	// Utworzenie rozkładu równomiernego dla zakresu od (0,0) do (_sizeX,_sizeY)
	uniform_int_distribution<int> distributionX(0, this->_sizeX-1);
	uniform_int_distribution<int> distributionY(0, this->_sizeY-1);

	// Wylosowanie liczby
	int maxTries = _placingBonusTries;
	int rand_x;
	int rand_y;

	for (int i = 0; i < num; i++)
	{
		//losuj współrzędne tak długo jak nie znajdziesz pustych, maxTries razy
		int ithTry = 0;
		do {
			ithTry++;
			rand_x = distributionX(generator);
			rand_y = distributionY(generator);
		} while (_gameMap[rand_y][rand_x] != EMPTY_SPRITE && ithTry <= maxTries);

		if (ithTry >= maxTries) continue;
		
		_gameMap[rand_y][rand_x] = BONUS_SPRITE;
	}
}


void Game::removeSnake(Player* player)
{
	for (Point p : player->sprite) {
		int y = p.posY, x = p.posX;
		if (_gameMap[y][x] == getPlayerASCII(player) ||
		_gameMap[y][x] == PLAYER_HEAD_SPRITE)
		{
		_gameMap[y][x] = BONUS_SPRITE;
		}
	}
}


void Game::drawSnakeHead(Player* player)
{
	if (player->isPlaying)
	{
		_gameMap[player->sprite.front().posY][player->sprite.front().posX] = PLAYER_HEAD_SPRITE;
	}
	
}


void Game::movePlayer(Player* player) 
{
	if (!player->isPlaying)
		return;
	//Przesuń jego postać
	int xTranslation = 0;
	int yTranslation = 0;
	int step = _playerStep;

	switch (player->currentDirection) {
	case UP:
		xTranslation = 0;
		yTranslation = -1 * step;;
		break;
	case DOWN:
		xTranslation = 0;
		yTranslation = +1 * step;
		break;
	case LEFT:
		xTranslation = -1 * step;
		yTranslation = 0;
		break;
	case RIGHT:
		xTranslation = +1 * step;
		yTranslation = 0;
		break;
	default:
		//printf("\n Critical 'player->currentDirection' error ! \n");
		break;
	}

	// (dodaj na początek listy nowe kordy, a ogon zwiń) i Rozwiąż konfilikty
	Point currHeadPos = player->sprite.front();
	Point currTailPos = player->sprite.back();
	Point nextHeadPos;

	nextHeadPos.posX = currHeadPos.posX + xTranslation;
	if (nextHeadPos.posX < 0) nextHeadPos.posX += _sizeX;
	if (nextHeadPos.posX >= _sizeX) nextHeadPos.posX -= _sizeX;

	nextHeadPos.posY = currHeadPos.posY + yTranslation;
	if (nextHeadPos.posY < 0) nextHeadPos.posY += _sizeY;
	if (nextHeadPos.posY >= _sizeY) nextHeadPos.posY -= _sizeY;

	// reackja na zawartość następnego pola
	char nextPos = _gameMap[nextHeadPos.posY][nextHeadPos.posX];
	char playerASCII = getPlayerASCII(player);

	if (nextPos == EMPTY_SPRITE || nextPos == playerASCII) {
		player->sprite.push_front(nextHeadPos);
		player->sprite.pop_back();
		_gameMap[currTailPos.posY][currTailPos.posX] = EMPTY_SPRITE;
	}
	else if (nextPos == BONUS_SPRITE) {
		player->score += _bonusScoreInc;
		player->sprite.push_front(nextHeadPos);
	}
	else {
		// on the next position enemy snake is placed
		player->isPlaying = false;
		removeSnake(player);
	}

	// Nanieś węża na mapę
	if (player->isPlaying)
	{
		printSnake(player);
	}

	//sprawdz warunek końca gry
	if (player->score >= _winScore) {
		winnerID = player->ID;
		_gameState = false;
		printf("Gracz %d wygral", winnerID);
	}
	
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

int Game::getWinnerID()
{
	return winnerID;
}

Game::~Game() 
{
	clearMap(_gameMap);
}
