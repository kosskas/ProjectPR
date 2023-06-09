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
    game = new Game(players, _setup.mapSizeY, _setup.mapSizeX);
    _isServerRunning = true;
    mapMsgSize = _setup.mapSizeY * _setup.mapSizeX;
   
    mapBuffer = new char[mapMsgSize];
    printf("Mapa bajtow %d ", mapMsgSize);
}


Server::~Server()
{
    printf("\n Closing server... \n");

    _isServerRunning = false;

    //CloseHandle(SenderTh);

    WaitForSingleObject(PingerTh, INFINITE);
    CloseHandle(PingerTh);

    WaitForSingleObject(SenderTh, INFINITE);
    CloseHandle(SenderTh);

    for (Player* player : players) {
        deletePlayer(player);
    }

    shutdownSocket(_socket);

    closeSocket(_socket);

    WSACleanup();

    printf("Server closed \n");
}


unsigned int Server::setPlayerID()
{
    return (unsigned int)players.size();
}


void Server::waitForPlayers()
{
    SOCKET PlayerSocket;
    sockaddr_in sc;
    int lenc = sizeof(sc);

    printf("\t JoinPlayer: start \n");

    while (_isServerRunning && players.size() < _setup.maxNumberOfClients) {

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
        player->ID = setPlayerID();
        player->score = 0;

        players.push_back(player);

        char ipStr[16];
        inet_ntop(AF_INET, &(player->ip), ipStr, sizeof ipStr);
        printf("JoinPlayer: Player(%d) %s:%d joined successfully \n", player->ID, ipStr, player->port);
    }

    printf("\t JoinPlayer: stop \n");
}


void Server::deletePlayer(Player* player)
{
    player->isRunning = false;
    WaitForSingleObject(player->thHandle, INFINITE);
    char ipStr[16];
    inet_ntop(AF_INET, &(player->ip), ipStr, sizeof ipStr);
    printf("Deleting Player(%d) %s:%d \n", player->ID, ipStr, player->port);

    shutdownSocket(player->sock);
    closeSocket(player->sock);
    CloseHandle(player->thHandle);
    delete player;
}

void Server::endConnection()
{
    //  # TODO
}


void Server::initGarbageCollector()
{
    PingerTh = CreateThread(NULL, 0, &Pinger, (void*)this, 0, pingerThID);
    if (PingerTh == NULL) {
        printf("Failed to create 'PingerTh' thread.\n");
        return;
    }
}


void Server::initMapBroadcast()
{
    SenderTh = CreateThread(NULL, 0, &Broadcast, (void*)this, 0, senderThID);
    if (SenderTh == NULL) {
        printf("Failed to create 'SenderTh' thread.\n");
        return;
    }    
}


void Server::run()
{
    initGarbageCollector();
    //
    waitForPlayers();
    //wszyscy gracze dołączyli

    //initMapBroadcast();

    ///Pętla grająca
    //IF LICZBA GRACZY == 0 END

    while (_isServerRunning /* && game.checkGameState()*/ && !players.empty()) {
        for (Player* Player : players) {
            printf("0x%2X ", Player->currentDirection);
            //game.ruszGraczem(*Player)
        }
        //game.rozstawBonusy();
        Sleep(200); //jako param
        sendMap();
        //wyślijMapę()?
    }
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

    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    char msg[4];
    codeMessage(player, msg, CONN);
    int iSendResult = send(player->sock, msg, 4, 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("%d send failed with error: %d\n", __LINE__, WSAGetLastError());
        player->isRunning = false;
        return 0;
    }

    // Receive until the peer shuts down the connection
    do {
        char recvbuf[DEFAULT_BUFLEN] = { 0 };

        iResult = recv(player->sock, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            //printf("ClientListener: Player(%d) Recevied: %s \n", player->ID, recvbuf);
            player->currentDirection = recvbuf[0];
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


DWORD __stdcall Broadcast(LPVOID param)
{
    Server* server = (Server*)param;

    printf("\t Broadcast:  start \n");

    while (server->_isServerRunning) {
        Sleep(300); //jako param
        server->game->getMap(server->mapBuffer);

        if (server->players.size() > 0)
            printf("Broadcast: Players in game: %d \n", server->players.size());
        for (Player* Player : server->players) {
            if (Player->sock != NULL) {
                int iSendResult = send(Player->sock, server->mapBuffer, server->mapMsgSize, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("%d | Broadcast: send to Player(%d) failed with error: %d\n", __LINE__, Player->ID, WSAGetLastError());
                }
            }
        }
    }

    printf("\t Broadcast:  stop \n");

    return 0;
}


void Server::sendMap()
{  
    game->getMap(mapBuffer+2);
    if (players.size() > 0)
        printf("Broadcast: Players in game: %d \n", players.size());
    for (Player* Player : players) {
        if (Player->sock != NULL) {
            codeMessage(Player, mapBuffer, MAP);
            int iSendResult = send(Player->sock, mapBuffer, mapMsgSize+3, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("%d | Broadcast: send to Player(%d) failed with error: %d\n", __LINE__, Player->ID, WSAGetLastError());
            }
        }
    }
}

void codeMessage(Player* player, char* msg, MSGMODE mode)
{
    switch (mode) {
    case CONN:
        msg[0] = mode;
        msg[1] = player->srvptr->getXSize();
        msg[2] = player->srvptr->getYSize();
        msg[3] = player->ID;
        break;
    case DISC:
        msg[0] = mode;
        msg[1] = player->score;
        break;
    case END:
    case SPECTATE:
    case MAP:
        msg[0] = mode;
        msg[1] = player->score;
        break;
    default:
        break;
    }
}


DWORD __stdcall Pinger(LPVOID param)
{
    Server* server = (Server*)param;

    printf("\t Pinger:  start \n");
    while (server->_isServerRunning) {
        Sleep(1334); //jako param

        server->players.remove_if([server](Player* player) {
            bool isPlayerInactive = (send(player->sock, NULL, 0, 0) == SOCKET_ERROR || player->isRunning == false);
            if (isPlayerInactive) {
                printf("Pinger:  Removing inactive Player(%d) \n", player->ID);
                server->deletePlayer(player);
            }
            return isPlayerInactive;
        });

        //printf("Pinger\n");
    }

    printf("\t Pinger:  stop \n");

    return 0;
}
