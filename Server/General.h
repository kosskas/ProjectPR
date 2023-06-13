#ifndef GENERAL
#define GENERAL


#undef UNICODE
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <stdint.h>
#include <random>
#include <fstream>
#include <string>


#define LEFT VK_LEFT
#define RIGHT VK_RIGHT
#define UP VK_UP
#define DOWN VK_DOWN


// -----------------------------------------
#define CONFIG_FILE "..\\server_config.txt"
#define SEPARATOR '='

#define _DEFAULT_BACKLOG_LENGTH SOMAXCONN
#define _DEFAULT_BONUS_SCORE_INC 10
#define _DEFAULT_MAP_SIZE_X 100 
#define _DEFAULT_MAP_SIZE_Y 25
#define _DEFAULT_NUMBER_OF_BONUSES_AT_ONCE 3
#define _DEFAULT_NUMBER_OF_PLAYERS 2
#define _DEFAULT_PORT 27015
#define _DEFAULT_SLEEP_MS_END_CONNECTION 2000
#define _DEFAULT_SLEEP_MS_PINGER 500
#define _DEFAULT_SLEEP_MS_RUN 200
#define _DEFAULT_TIMESPAN_BETWEEN_BONUSES 20
#define _DEFAULT_WIN_SCORE 50

#define _DEFAULT_PLACING_BONUS_TRIES 5
#define _DEFAULT_PLAYER_STEP 1


#define _DEFAULT_EMPTY_SPRITE ' '
#define _DEFAULT_BONUS_SPRITE 'X'
#define _DEFAULT_PLAYER_HEAD_SPRITE '#'
// -----------------------------------------


//#define BUFFER_SIZE 4096
#define CLIENT_LISTENER_RECV_BUFFER_SIZE 4
#define CONN_MESSAGE_BUFFER_SIZE 5
#define DISC_MESSAGE_BUFFER_SIZE 4


#define EMPTY_SPRITE ' '
#define BONUS_SPRITE 'X'
#define PLAYER_HEAD_SPRITE '#'


#pragma comment (lib, "Ws2_32.lib")


using std::list;
using std::uniform_int_distribution;
using std::random_device;
using std::mt19937;

#define Terminate(a) TerminateThread(a, 0)
#endif // !GENERAL