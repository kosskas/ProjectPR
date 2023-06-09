#include "Client.h"
#include <iostream>


// - - - - - - - - - - Client :: protected - - - - - - - - - - \\


bool Client::startUpWinsock()
{
    WORD wsaVersion = MAKEWORD(2, 2);
    WSADATA wsaData = { 0 };

    if (WSAStartup(wsaVersion, &wsaData) == 0) {
        //printf("WSA started up successfully \n");
        return true;
    }
    else {
        printf("WSA starting up failed \n");
        return false;
    }
}


bool Client::createSocket()
{
    _socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (_socket == INVALID_SOCKET) {
        printf("Socket creating failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        //printf("Socket created successfully \n");
        return true;
    }
}

bool Client::connectToServer()
{
    addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(_setup.serverIP, _setup.serverPort, &hints, &result); //podstawić ipadress
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return false;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Connect to server.
        iResult = connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(_socket);
            _socket = INVALID_SOCKET;
            createSocket();
            continue;
        }
        
        freeaddrinfo(result);
        return true;
    }

    freeaddrinfo(result);
    return false;
}


bool Client::disconnectFromServer()
{
    if (!shutdownSocket()) {
        return false;
    }

    if (!closeSocket()) {
        return false;
    }
    
    return true;
}


bool Client::shutdownSocket()
{
    if (shutdown(_socket, SD_BOTH) == SOCKET_ERROR) {
        printf("Socket shutdown failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        // printf("Socket shutdown successfully \n");
        return true;
    }
}


bool Client::closeSocket()
{
    if (closesocket(_socket) == SOCKET_ERROR) {
        printf("Socket close failed: error = %d \n", WSAGetLastError());
        return false;
    }
    else {
        // printf("Socket closed successfully \n");
        return true;
    }
}


int Client::initConsole()
{
    //Standardowe wejście
    _hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (_hStdin == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle err");
        return 1;
    }

    if (!GetConsoleMode(_hStdin, &_fdwSaveOldInMode)) {
        printf("GetConsoleMode err");
        SetConsoleMode(_hStdin, _fdwSaveOldInMode);
        return 1;
    }

    _fdwInMode = _fdwSaveOldInMode | ENABLE_WINDOW_INPUT | ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS;
    if (!SetConsoleMode(_hStdin, _fdwInMode)) {
        printf("SetConsoleMode err");
        SetConsoleMode(_hStdin, _fdwSaveOldInMode);
        return 1;
    }

    //Standardowe wyjście
    _hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (_hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle err");
        return 1;
    }

    if (!GetConsoleMode(_hStdout, &_fdwSaveOldOutMode)) {
        printf("GetConsoleMode err");
        SetConsoleMode(_hStdout, _fdwSaveOldOutMode);
        return 1;
    }

    _fdwOutMode = _fdwSaveOldOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(_hStdout, _fdwOutMode)) {
        printf("SetConsoleMode err");
        SetConsoleMode(_hStdout, _fdwSaveOldOutMode);
        return 1;
    }
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(_hStdout, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(_hStdout, &cursorInfo);

    return 0;
}


bool Client::initReceiver()
{
    _MsgReceiverTh = CreateThread(NULL, 0, &MsgReceiverListener, (void*)this, 0, _MsgReceiverThID);
    if (_MsgReceiverTh == NULL) {
        printf("Failed to create MsgReceiverListener thread.\n");
        return false;
    }
    return true;
}


bool Client::initSender()
{
    _MsgSenderTh = CreateThread(NULL, 0, &MsgSender, (void*)this, 0, _MsgSenderThID);
    if (_MsgSenderTh == NULL) {
        printf("Failed to create MsgSender thread.\n");
        return false;
    }
    return true;
}


bool Client::initKeyEventListener()
{
    _KeyEventListenerTh = CreateThread(NULL, 0, &KeyEventListener, (void*)this, 0, _KeyEventListenerThID);
    if (_KeyEventListenerTh == NULL) {
        printf("Failed to create KeyEventListener thread.\n");
        return false;
    }
    return true;
}


bool Client::sendMessage(const char* sendbuf, int len) 
{
    int iResult = send(_socket, sendbuf, len, 0);
    if (iResult == SOCKET_ERROR) {
        printf("Line %d in function %s\tsend failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
        return false;
    }
    //printf("Wyslano %ldB\n", iResult);
    return true;
}

void Client::printGame(const char* map)
{
    if (_mapSizeX == 0xFFFFFFFF)
        return;
    //    //fflush(stdout);       ???? 
    printf("\033[0;0H");
    for (unsigned int y = 0; y < _mapSizeY; y++) {
        for (unsigned int x = 0; x < _mapSizeX; x++) {
            printf("\033[1;34m%c\033[0m", _recvbuf[y + _mapSizeY * x]);
        }
        printf("\n");
    }
    char arrow = getArrow(_keyInput);
    printf("Destroy \033[1;31mall\033[0m enemies. Use \x18\x19\x1a< to navigate\n");
    printf("Your number %d   Current direction: \033[1;33m%c \033[0m   Your score %d",_playerID, arrow, _playerScore);

}

char Client::getArrow(char direction) 
{
    switch (direction) {
    case VK_LEFT:
        return '<';
    case VK_RIGHT:
        return '\x1a';
    case VK_UP:
        return '\x18';
    case VK_DOWN:
        return '\x19';
    default:
        return '?';
    }
}


// - - - - - - - - - - Client :: public - - - - - - - - - - \\


Client::Client(ClientSetup setup) :
    _setup(setup),
    _mapSizeX(setup.mapSizeX),
    _mapSizeY(setup.mapSizeY),
    _isRunning(true), 
    _isConnected(false)
{
    if (!startUpWinsock()) {
        ExitProcess(1);
    }

    if (!createSocket()) {
        WSACleanup();
        ExitProcess(1);
    }

    if (!connectToServer()) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        ExitProcess(1);
    }

    _isConnected = true;

    if (initConsole() != 0) {
        if (_fdwSaveOldInMode != 0)
            SetConsoleMode(_hStdin, _fdwSaveOldInMode);
        if (_fdwSaveOldOutMode != 0)
            SetConsoleMode(_hStdout, _fdwSaveOldOutMode);
        ExitProcess(1);
    }
}


void Client::run()
{
    if (!initReceiver())
        return;

    if (!initSender())
        return;

    if (!initKeyEventListener())
        return;

    int licz = 300;
    while (licz && _isRunning) {
        Sleep(100);
        licz--;

    }
}


Client::~Client() 
{
    _isRunning = false;

    SetConsoleMode(_hStdin, _fdwSaveOldInMode);
    SetConsoleMode(_hStdout, _fdwSaveOldOutMode);

    // CloseHandle(MsgSender);

    WaitForSingleObject(_MsgSenderTh, INFINITE);
    CloseHandle(_MsgSenderTh);

    WaitForSingleObject(_MsgReceiverTh, INFINITE);
    CloseHandle(_MsgReceiverTh);

    WaitForSingleObject(_KeyEventListenerTh, INFINITE);
    CloseHandle(_KeyEventListenerTh);

    if (disconnectFromServer()) {
        _isConnected = false;
    }

    WSACleanup();
}

void Client::decodeMessage() {
    char mode = _recvbuf[0];
    switch (mode) {
    case Client::CONN:
        _mapSizeX = _recvbuf[1];
        _mapSizeY = _recvbuf[2];
        _playerID = _recvbuf[3];
        break;
    case Client::DISC:
        _playerScore = _recvbuf[1];
        _isRunning = false;
        break;
    case Client::END:
        _isRunning = false;
        break;
    case Client::SPECTATE:
        //TODO NA KOŃCU
        break;
    case Client::MAP:
        _playerScore = _recvbuf[1];
        printGame(_recvbuf + 2);
        break;
    default:
        break;
    }
}




// - - - - - - - - - - threads functions - - - - - - - - - - \\


DWORD __stdcall MsgReceiverListener(LPVOID param) 
{
    Client* client = (Client*)param;
    int iResult;

    do {
        iResult = recv(client->_socket, client->_recvbuf, client->_recvbuflen, 0);
        if (iResult > 0) {
            client->decodeMessage();
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            client->_isRunning = false;
        }
        else {
            printf("Line %d in function %s \t recv failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
            client->_isRunning = false;
        }

    } while (client->_isRunning);

    return 0;
}


DWORD __stdcall MsgSender(LPVOID param)
{
    Client* client = (Client*)param;

    unsigned char lastKeyInput = 0;
    

    do { 
        if (lastKeyInput != client->_keyInput) {
            lastKeyInput = client->_keyInput;
            const char* currentKeyInput = (const char*)&(client->_keyInput);
            client->sendMessage(currentKeyInput, 1);
            
        }

        Sleep(100);

    } while (client->_isRunning);

    client->sendMessage("END", 4);

    return 0;
}


DWORD __stdcall KeyEventListener(LPVOID param)
{
    Client* client = (Client*)param;
    DWORD cNumRead;
    INPUT_RECORD irInBuf[128];

    while (client->_isRunning) {

        if (!ReadConsoleInput(client->_hStdin, irInBuf, 128, &cNumRead)) { 
            printf("ReadConsoleInput");
            return 1;
        }

        for (unsigned int i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {

                switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_LEFT:
                    client->_keyInput = VK_LEFT;
                    break;
                case VK_RIGHT:
                    client->_keyInput = VK_RIGHT;
                    break;
                case VK_UP:
                    client->_keyInput = VK_UP;
                    break;
                case VK_DOWN:
                    client->_keyInput = VK_DOWN;
                    break;
                case VK_Q:
                    client->_isRunning = false;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return 0;
}