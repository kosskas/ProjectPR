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

bool Client::connectToServer(const char* ipadress)
{
    addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipadress, DEFAULT_PORT, &hints, &result); //podstawić ipadress
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
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle err");
        return 1;
    }

    if (!GetConsoleMode(hStdin, &fdwSaveOldInMode)) {
        printf("GetConsoleMode err");
        SetConsoleMode(hStdin, fdwSaveOldInMode);
        return 1;
    }

    fdwInMode = fdwSaveOldInMode | ENABLE_WINDOW_INPUT | ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS;
    if (!SetConsoleMode(hStdin, fdwInMode)) {
        printf("SetConsoleMode err");
        SetConsoleMode(hStdin, fdwSaveOldInMode);
        return 1;
    }

    //Standardowe wyjście
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle err");
        return 1;
    }

    if (!GetConsoleMode(hStdout, &fdwSaveOldOutMode)) {
        printf("GetConsoleMode err");
        SetConsoleMode(hStdout, fdwSaveOldOutMode);
        return 1;
    }

    fdwOutMode = fdwSaveOldOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hStdout, fdwOutMode)) {
        printf("SetConsoleMode err");
        SetConsoleMode(hStdout, fdwSaveOldOutMode);
        return 1;
    }

    return 0;
}


bool Client::initReceiver()
{
    MsgReceiverTh = CreateThread(NULL, 0, &MsgReceiverListener, (void*)this, 0, MsgReceiverThID);
    if (MsgReceiverTh == NULL) {
        printf("Failed to create MsgReceiverListener thread.\n");
        return false;
    }
    return true;
}


bool Client::initSender()
{
    MsgSenderTh = CreateThread(NULL, 0, &MsgSender, (void*)this, 0, MsgSenderThID);
    if (MsgSenderTh == NULL) {
        printf("Failed to create MsgSender thread.\n");
        return false;
    }
    return true;
}


bool Client::initKeyEventListener()
{
    KeyEventListenerTh = CreateThread(NULL, 0, &KeyEventListener, (void*)this, 0, KeyEventListenerThID);
    if (KeyEventListenerTh == NULL) {
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

void Client::printGame()
{
    printf("Destroy \033[1;31mall\033[0m enemies. Use \x18\x19\x1a< to navigate\n");
    char arrow = getArrow(keyInput);
    printf("\tCurrent direction: \033[1;33m%c\033[0m\n", arrow); //odebrano wyslano???

    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            printf("\033[1;34m%c\033[0m", recvbuf[y + 10 * x]);
        }
        printf("\n");
    }

    printf("\033[0;0H");
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


Client::Client(const char* ipadress) :
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

    if (!connectToServer(ipadress)) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        ExitProcess(1);
    }

    _isConnected = true;

    if (initConsole() != 0) {
        if (fdwSaveOldInMode != 0)
            SetConsoleMode(hStdin, fdwSaveOldInMode);
        if (fdwSaveOldOutMode != 0)
            SetConsoleMode(hStdout, fdwSaveOldOutMode);
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

    SetConsoleMode(hStdin, fdwSaveOldInMode);
    SetConsoleMode(hStdout, fdwSaveOldOutMode);

    // CloseHandle(MsgSender);

    WaitForSingleObject(MsgSenderTh, INFINITE);
    CloseHandle(MsgSenderTh);

    WaitForSingleObject(MsgReceiverTh, INFINITE);
    CloseHandle(MsgReceiverTh);

    WaitForSingleObject(KeyEventListenerTh, INFINITE);
    CloseHandle(KeyEventListenerTh);

    if (disconnectFromServer()) {
        _isConnected = false;
    }

    WSACleanup();
}


// - - - - - - - - - - threads functions - - - - - - - - - - \\


DWORD __stdcall MsgReceiverListener(LPVOID param) 
{
    Client* client = (Client*)param;
    int iResult;

    do {
        iResult = recv(client->_socket, client->recvbuf, client->recvbuflen, 0);
        if (iResult > 0) {
            if (compareString(client->recvbuf, "END")) {
                client->_isRunning = false;
            }
            else {
                //fflush(stdout);       ????   
                client->printGame();
                //printf("\nOtrzymano: %dB\t", iResult);
                //printf("Wynik: %s\n", client->recvbuf);
            }
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
        if (lastKeyInput != client->keyInput) {
            lastKeyInput = client->keyInput;
            const char* currentKeyInput = (const char*)&(client->keyInput);
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

        if (!ReadConsoleInput(
            client->hStdin,     // input buffer handle 
            irInBuf,            // buffer to read into 
            128,                // size of read buffer 
            &cNumRead)          // number of records read 
            ) { 
            printf("ReadConsoleInput");
            return 1;
        }

        for (unsigned int i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {

                switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode)
                {
                case VK_LEFT:
                    client->keyInput = VK_LEFT;
                    break;
                case VK_RIGHT:
                    client->keyInput = VK_RIGHT;
                    break;
                case VK_UP:
                    client->keyInput = VK_UP;
                    break;
                case VK_DOWN:
                    client->keyInput = VK_DOWN;
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