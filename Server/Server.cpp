#include "Server.h"

// - - - - - - - - - - Server :: protected - - - - - - - - - - \\


bool Server::startUpWinsock()
{
    WORD wsaVersion = MAKEWORD(2, 2);
    WSADATA wsaData = { 0 };

    if (WSAStartup(wsaVersion, &wsaData) == 0) {
        printf("WSA started up successfully \n");
        return true;
    }
    else {
        printf("WSA starting up failed \n");
        return false;
    }
}


bool Server::createSocket()
{
    _socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (_socket == INVALID_SOCKET) {
        printf("Socket creating failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        printf("Socket created successfully \n");
        return true;
    }
}


bool Server::bindSocket()
{
    sockaddr_in service = { 0 };
    service.sin_family = AF_INET;
    service.sin_port = htons(_setup.port);
    service.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        printf("Socket binding failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        printf("Socket bound successfully \n");
        return true;
    }
}


bool Server::startListen()
{
    if (listen(_socket, _setup.backlog) == SOCKET_ERROR) {
        printf("Starting server listening failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        printf("Server is listening \n");
        return true;
    }
}


bool Server::shutdownSocket(SOCKET sock)
{
    if (shutdown(sock, SD_BOTH) == SOCKET_ERROR) {
        printf("Socket shutdown failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        printf("Socket shutdown successfully \n");
        return true;
    }
}


bool Server::closeSocket(SOCKET sock)
{
    if (closesocket(sock) == SOCKET_ERROR) {
        printf("Socket close failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        printf("Socket closed successfully \n");
        return true;
    }
}


// - - - - - - - - - - Server :: public - - - - - - - - - - \\


Server::Server(ServerSetup setup)
    : _setup(setup)
{
    if (!startUpWinsock()) {
        ExitProcess(1);
    }

    if (!createSocket()) {
        WSACleanup();
        ExitProcess(1);
    }

    if (!bindSocket()) {
        WSACleanup();
        ExitProcess(1);
    }

    if (!startListen()) {
        WSACleanup();
        ExitProcess(1);
    }

    _isServerRunning = true;
    _mapMsgSize = _setup.mapSizeY * _setup.mapSizeX;
   
    _mapBuffer = new char[_mapMsgSize];
    printf("Mapa bajtow %d ", _mapMsgSize);
}


Server::~Server()
{
    printf("\n Closing server... \n");
    _isServerRunning = false;
    delete _game;

    TerminateThread(_PingerTh, 0);
    DWORD res =  WaitForSingleObject(_PingerTh, INFINITE);
    if (res == WAIT_OBJECT_0) {
        printf("OK");
    }
    else {
        printf("NOT OK");
    }
    CloseHandle(_PingerTh);


    shutdownSocket(_socket);

    closeSocket(_socket);

    WSACleanup();
    printf("Server closed \n");

}


unsigned int Server::setPlayerID()
{
    return (unsigned int)_players.size();
}


void Server::waitForPlayers()
{
    SOCKET PlayerSocket;
    sockaddr_in sc;
    int lenc = sizeof(sc);

    printf("\t JoinPlayer: start \n");

    while (_isServerRunning && _players.size() < _setup.maxNumberOfClients) {

        PlayerSocket = accept(_socket, (sockaddr*)&sc, &lenc);
        if (PlayerSocket == INVALID_SOCKET) {
            printf("JoinPlayer: Accept failed with error: %d \n", WSAGetLastError());
        }

        DWORD PlayerThreadId;
        Player* player = new Player;

        HANDLE PlayerThread = CreateThread(NULL, 0, &ClientListener, player, 0, &PlayerThreadId);
        if (PlayerThread == NULL) {
            printf("JoinPlayer: Failed to create thread. \n");
            shutdownSocket(PlayerSocket);
            closeSocket(PlayerSocket);
            continue;
        }
        player->srvptr = this;
        player->sock = PlayerSocket;
        player->ip = sc.sin_addr;
        player->port = sc.sin_port;
        player->thHandle = PlayerThread;
        player->thId = PlayerThreadId;
        player->isRunning = true;
        player->isPlaying = true;
        player->ID = setPlayerID();
        player->score = 0;

        _players.push_back(player);

        char ipStr[16];
        inet_ntop(AF_INET, &(player->ip), ipStr, sizeof ipStr);
        printf("JoinPlayer: Player(%d) %s:%d joined successfully \n", player->ID, ipStr, player->port);
    }

    printf("\t JoinPlayer: stop \n");
}


void Server::deletePlayer(Player* player)
{
    player->isRunning = false;
    player->isPlaying = false;

    //TerminateThread(player->thHandle, 0);
    DWORD res = WaitForSingleObject(player->thHandle, INFINITE);
    if (res == WAIT_OBJECT_0) {
        printf("player->thHandle OK");
    }
    else {
        printf("player->thHandle NOT OK");
    }
    //WaitForSingleObject(player->thHandle, INFINITE);
    CloseHandle(player->thHandle);
    
    char ipStr[16];
    inet_ntop(AF_INET, &(player->ip), ipStr, sizeof ipStr);
    printf("Deleting Player(%d) %s:%d \n", player->ID, ipStr, player->port);

    shutdownSocket(player->sock);
    closeSocket(player->sock);
    delete player;
    player = nullptr;
    
}

void Server::endConnection()
{
    char msg[4];
    for (Player* Player : _players) {
        if (Player->sock != NULL) {
            codeMessage(Player, msg, DISC); //WON
            int iSendResult = send(Player->sock, msg, 4, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("%d | Broadcast: send to Player(%d) failed with error: %d\n", __LINE__, Player->ID, WSAGetLastError());
            }
        }
    }
}


void Server::initGarbageCollector()
{
    _PingerTh = CreateThread(NULL, 0, &Pinger, (void*)this, 0, _pingerThID);
    if (_PingerTh == NULL) {
        printf("Failed to create 'PingerTh' thread.\n");
        return;
    }
}


void Server::run()
{
    initGarbageCollector();
    //initJoiner();
    waitForPlayers();
    //wszyscy gracze dołączyli
    //while (!_canStartGame);


    _game = new Game(_players, _setup.mapSizeY, _setup.mapSizeX);

    ///Pętla grająca
    //IF LICZBA GRACZY == 0 END

    // Inicjalizacja generatora liczb losowych dla bonusów
    //std::random_device rd;
    //std::mt19937 generator(rd());
    //std::uniform_int_distribution<int> bonusRandTime(MIN_TIME_BETWEEN_BONUS_MS, MAX_TIME_BETWEEN_BONUS_MS);
    unsigned int i = 0;

    while (_isServerRunning /* && game.checkGameState()*/ && !_players.empty()) { // && liczba grających > 1

        for (Player* player : _players) {
            //_game->removeSnake(player);
            _game->movePlayer(player);
        }

        for (Player* player : _players) {
            _game->drawSnakeHead(player);
        }
 
        if (i % 16 == 0) _game->placeBonuses(1);
        
        Sleep(200); //jako param
        sendMessage();
        i++;
    }
    //endConnection();
}

unsigned int Server::getXSize()
{
    return _setup.mapSizeX;
}

unsigned int Server::getYSize()
{
    return _setup.mapSizeY;
}


// - - - - - - - - - - threads functions - - - - - - - - - - \\

DWORD __stdcall ClientListener(LPVOID param)
{
    const int DEFAULT_BUFLEN = 4;
    Player* player = (Player*)param;
    printf("ClientListener: Player(%d) start \n", player->ID);

    int _recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    char msg[4];
    codeMessage(player, msg, Server::CONN);
    int iSendResult = send(player->sock, msg, 4, 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("%d send failed with error: %d\n", __LINE__, WSAGetLastError());
        player->isRunning = false;
        return 0;
    }

    // Receive until the peer shuts down the connection
    do {
        char _recvbuf[DEFAULT_BUFLEN] = { 0 };

        iResult = recv(player->sock, _recvbuf, _recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            //printf("ClientListener: Player(%d) Recevied: %s \n", player->ID, _recvbuf);
            player->currentDirection = _recvbuf[0];
        }
        else if (iResult == 0) {
            printf("Connection closing...\n");
            break;
        }
        else {
            printf("%d recv failed with error: %d\n", __LINE__, WSAGetLastError());
            break;
        }

    } while (iResult > 0 && player->isRunning);

    printf("ClientListener: Player(%d) stop \n", player->ID);
    player->isRunning = false;
    return 0;
}

void Server::sendMessage()
{  
    //printf("wysylanie");
    if (_players.empty())
        return;
    _game->getMap(_mapBuffer+3);
    if (_players.size() > 0) {
        //printf("Broadcast: Players in game: %d \n", _players.size());
    }        
    for (Player* Player : _players) {
        if (Player->sock != NULL) {
            if(Player->isPlaying)
                codeMessage(Player, _mapBuffer, MAP);
            else
                codeMessage(Player, _mapBuffer, SPECTATE);
            int iSendResult = send(Player->sock, _mapBuffer, _mapMsgSize+3, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("%d | Broadcast: send to Player(%d) failed with error: %d\n", __LINE__, Player->ID, WSAGetLastError());
            }

        }
    }
}

void codeMessage(Player* player, char* msg, Server::MSGMODE mode)
{
    
    switch (mode) {
    case Server::CONN:
        msg[0] = mode;
        msg[1] = player->srvptr->getXSize();
        msg[2] = player->srvptr->getYSize();
        msg[3] = player->ID;
        break;
    case Server::DISC:
        msg[0] = mode;      
        //wyślij wszystkim id gracza który wygrał
        break;      
    case Server::SPECTATE:
    case Server::MAP:
    {
        short score = player->score;
        msg[0] = mode;
        __asm {
            push eax
            push ebx
            mov ebx, msg
            mov ax, score
            mov [ebx + 1], al
            mov [ebx + 2], ah
            pop ebx
            pop eax
        };
        break;
    }
    default:
        break;
    }
}


DWORD __stdcall Pinger(LPVOID param)
{
    Server* server = (Server*)param;

    printf("\t Pinger:  start \n");
    while (server->_isServerRunning) {
  
        //printf("\t Online %d\n", server->_players.size());
        std::list<Player*>::iterator it = server->_players.begin();
        while (it != server->_players.end())
        {
            bool isInactive = ((*it)->isRunning == false || send((*it)->sock, NULL, 0, 0) == SOCKET_ERROR );

            if (isInactive) {
                printf("Pinger:  Removing inactive Player(%d) \n", (*it)->ID);
                (*it)->isRunning = false;
                server->_game->removeSnake((*it));             
                server->deletePlayer((*it));
                it = server->_players.erase(it);
                
            }
            else {
                ++it;
            }
        }

        Sleep(500); //jako param
    }

    printf("\t Pinger:  stop \n");

    return 0;
}
