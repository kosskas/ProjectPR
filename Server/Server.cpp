#include "Server.h"

#define compareString(a, b) strcmp(a, b) == 0


// protected

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
// public

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
}


Server::~Server()
{
    printf("\n Closing server... \n");

    _isServerRunning = false;

    //CloseHandle(SenderTh);

    WaitForSingleObject(PingerTh, INFINITE);
    CloseHandle(PingerTh);

    for (Gamer* gamer : gamers) {
        deleteGamer(gamer);
    }

    shutdownSocket(_socket);

    closeSocket(_socket);

    WSACleanup();

    printf("Server closed \n");
}


void Server::run() {
    garbageCollector();
    waitForGamers();
    //initGame()
    sendMap(); //powinno być bardziej uruchom wątek wysyłania


    ///Pętla grająca
    while (_isServerRunning /*checkGameState*/) {
        for (Gamer* gamer : gamers) {
            printf("0x%2X ", gamer->currentDirection);
            //Game.ruszGraczem(gamer)
        }
        ///
        printf("\n");
        Sleep(500);
        //Sleep(100); //jakis czas
    }
}

unsigned int Server::setGamerID()
{
    return (unsigned int)gamers.size();
}

void Server::waitForGamers()
{
    SOCKET gamerSocket;
    sockaddr_in sc;
    int lenc = sizeof(sc);

    printf("\t JoinGamer: start \n");

    while (_isServerRunning && gamers.size() < _setup.maxNumberOfClients) {

        gamerSocket = accept(_socket, (sockaddr*)&sc, &lenc);
        if (gamerSocket == INVALID_SOCKET) {
            printf("JoinGamer: Accept failed with error: %d \n", WSAGetLastError());
        }

        DWORD gamerThreadId;
        Gamer* gamer = new Gamer;

        HANDLE gamerThread = CreateThread(NULL, 0, &ClientListener, gamer, 0, &gamerThreadId);
        if (gamerThread == NULL) {
            printf("JoinGamer: Failed to create thread. \n");
            shutdownSocket(gamerSocket);
            closeSocket(gamerSocket);
            continue;
        }
        gamer->sock = gamerSocket;
        gamer->ip = sc.sin_addr;
        gamer->port = sc.sin_port;
        gamer->thHandle = gamerThread;
        gamer->thId = gamerThreadId;
        gamer->isRunning = true;
        gamer->ID = setGamerID();


       gamers.push_back(gamer);

        char ipStr[16];
        inet_ntop(AF_INET, &(gamer->ip), ipStr, sizeof ipStr);
        printf("JoinGamer: Gamer(%d) %s:%d joined successfully \n", gamer->ID, ipStr, gamer->port);
    }

    printf("\t JoinGamer: stop \n");
}


void Server::deleteGamer(Gamer* gamer)
{
    char ipStr[16];
    inet_ntop(AF_INET, &(gamer->ip), ipStr, sizeof ipStr);
    printf("Deleting Gamer(%d) %s:%d \n", gamer->ID, ipStr, gamer->port);

    shutdownSocket(gamer->sock);
    closeSocket(gamer->sock);
    CloseHandle(gamer->thHandle);
    delete gamer;
}

void Server::endConnection()
{
    //  # TODO
}

void Server::garbageCollector() {
    PingerTh = CreateThread(NULL, 0, &Pinger, (void*)this, 0, pingerThID);
    if (PingerTh == NULL) {
        printf("Failed to create 'PingerTh' thread.\n");
        return;
    }
}

void Server::sendMap() {
    SenderTh = CreateThread(NULL, 0, &Broadcast, (void*)this, 0, senderThID);
    if (SenderTh == NULL) {
        printf("Failed to create 'SenderTh' thread.\n");
        return;
    }    
}


// threads functions 

DWORD __stdcall ClientListener(LPVOID param)
{
    const int DEFAULT_BUFLEN = 512;
    Gamer* gamer = (Gamer*)param;

    printf("ClientListener: Gamer(%d) start \n", gamer->ID);

    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    int iSendResult = send(gamer->sock, "CONNECTED", strlen("CONNECTED"), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("%d send failed with error: %d\n", __LINE__, WSAGetLastError());
        gamer->isRunning = false;
        return 0;
    }

    // Receive until the peer shuts down the connection
    do {
        char recvbuf[DEFAULT_BUFLEN] = { 0 };

        iResult = recv(gamer->sock, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            //printf("Bytes received: %d\n", iResult);
            printf("ClientListener: Gamer(%d) Recevied: %s \n", gamer->ID, recvbuf);
            gamer->currentDirection = recvbuf[0];
        }
        else if (iResult == 0) {
            printf("Connection closing...\n");
            break;
        }
        else {
            printf("%d recv failed with error: %d\n", __LINE__, WSAGetLastError());
            break;
        }

    } while (iResult > 0);

    printf("ClientListener: Gamer(%d) stop \n", gamer->ID);
    gamer->isRunning = false;
    return 0;
}


DWORD __stdcall Broadcast(LPVOID param)
{
    Server* server = (Server*)param;

    printf("\t Broadcast:  start \n");

    while (server->_isServerRunning) {
        Sleep(1000);
        const char* msg = "PING\0"; ///TU BĘDZIE WYSYŁANA MAPA

        if (server->gamers.size() > 0) printf("Broadcast: Gamers in game: %d \n", server->gamers.size());
        for (Gamer* gamer : server->gamers) {
            if (gamer->sock != NULL) {
                int iSendResult = send(gamer->sock, msg, strlen(msg), 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("%d | Broadcast: send to Gamer(%d) failed with error: %d\n", __LINE__, gamer->ID, WSAGetLastError());
                }
            }
        }
    }

    printf("\t Broadcast:  stop \n");

    return 0;
}


DWORD __stdcall Pinger(LPVOID param)
{
    Server* server = (Server*)param;

    printf("\t Pinger:  start \n");

    while (server->_isServerRunning) {
        Sleep(1334);

        server->gamers.remove_if([server](Gamer* gamer) {
            bool isGamerInactive = (send(gamer->sock, NULL, 0, 0) == SOCKET_ERROR || gamer->isRunning == false);
            if (isGamerInactive) {
                printf("Pinger:  Removing inactive Gamer(%d) \n", gamer->ID);
                server->deleteGamer(gamer);
            }
            return isGamerInactive;
        });

        //printf("Pinger\n");
    }

    printf("\t Pinger:  stop \n");

    return 0;
}