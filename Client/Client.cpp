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
    if (!_isRunning)
        return;
    char bufor[8];
    if (_mapSizeX == 0xFFFFFFFF)
        return;
    //    //fflush(stdout);       ???? 
    printf("\033[0;0H");
    for (unsigned int y = 0; y < _mapSizeY; y++) {
        for (unsigned int x = 0; x < _mapSizeX; x++) {
            //char tile = map[y + _mapSizeY * x];
            char tile = map[y*_mapSizeX + x];
            getColorById(bufor, tile);
            printf("%s%c", bufor, tile);
        }
        printf("\n");
    }
    printf("\033[0m");
    char arrow = getArrow(_keyInput);
    getColorById(bufor, (char)_playerID+'0');
    printf("Destroy \033[1;31mall\033[0m enemies. Use \x18\x19\x1a< to navigate\n");
    printf("Your number %s%d%s   Current direction: \033[1;33m%c \033[0m   Your score %d\n",bufor,_playerID,RESET, arrow, _playerScore);
    printf("\033[0m");
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
    _isRunning(true), 
    _isConnected(false)
{
    printf("Trying to connect to %s... \n", setup.serverIP);
    if (!startUpWinsock()) {
        ExitProcess(1);
    }

    if (!createSocket()) {
        WSACleanup();
        ExitProcess(1);
    }

    if (!connectToServer()) {
        printf("%sUnable to connect to server!%s\n",BRIGHT_RED, RESET);
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
    //system("cls");
}


void Client::run()
{
    if (!initReceiver())
        return;

    if (!initKeyEventListener())
        return;
    unsigned char lastKeyInput = 0;
    int licz = 300;
    while (_isRunning) {
        if (lastKeyInput != _keyInput) {
            lastKeyInput = _keyInput;
            const char* currentKeyInput = (const char*)&(_keyInput);
            sendMessage(currentKeyInput, 1);
        }
        Sleep(100); //jako parametr
    }
}


Client::~Client() 
{
    _isRunning = false;

    SetConsoleMode(_hStdin, _fdwSaveOldInMode);
    SetConsoleMode(_hStdout, _fdwSaveOldOutMode);

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
    char* msg = _recvbuf;
    short score = 0;
    switch (mode) {
    case Client::CONN:
        printf("%sCONNECTED%s\n", BRIGHT_GREEN, RESET);
        _mapSizeX = _recvbuf[1];
        _mapSizeY = _recvbuf[2];
        _playerID = _recvbuf[3];
        break;
    case Client::DISC:
        //_playerScore = _recvbuf[1];
        _isRunning = false;
        //odbierz id wygrywa i sprawdz czy to ty
        //printf("You won.\n");
        break;
    case Client::SPECTATE:
        printf("You lost. Now watch\n");
    case Client::MAP:
        __asm {
            push eax
            push ebx
            mov ebx, msg
            mov al, [ebx+1]
            mov ah, [ebx+2]
            mov score, ax
            pop ebx
            pop eax
        }
        _playerScore = score;
        printGame(_recvbuf + 3);
        break;
    case Client::BUSY:
        printf("Busy");
        _isRunning = false;
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
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(client->_socket, &readSet);

        struct timeval timeout;
        timeout.tv_sec = 2;  // 2 sekundy
        timeout.tv_usec = 0;

        // Wywołanie select
        int result = select(0, &readSet, nullptr, nullptr, &timeout);
        if (result == SOCKET_ERROR) {
            printf("Line %d in function %s \t select failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
            client->_isRunning = false;
            break;
        }

        if (result > 0) {
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
        }
        else if (result == 0) {
            printf("%sServer busy%s\n", RED, RESET);
            client->_isRunning = false;
            break;
        }

    } while (client->_isRunning);

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
                switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode) {
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
void Client::getColorById(char* buf, char id) {
    switch (id) {
    case '#':
        strcpy(buf, BG_WHITE);
        break;
    case '0':
        strcpy(buf, BG_GREEN);
        break;
    case '1':
        strcpy(buf, BG_YELLOW);
        break;
    case '2':
        strcpy(buf, BG_BLUE);
        break;
    case '3':
        strcpy(buf, BG_RED);
        break;
    case '4':
        strcpy(buf, BG_CYAN);
        break;
    case 'X':
        strcpy(buf, BG_LIGHT_RED);
        break;
    default:
        strcpy(buf, BG_LIGHT_GRAY);
        break;
    }
}