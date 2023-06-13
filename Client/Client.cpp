#include "Client.h"

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
    printf(RETURN_CURSOR);
    for (unsigned int y = 0; y < _mapSizeY; y++) {
        for (unsigned int x = 0; x < _mapSizeX; x++) {
            char tile = map[y*_mapSizeX + x];
            getColorById(bufor, tile);
            printf("%s%c", bufor, tile);
        }
        printf("\n");
    }
    printf(RESET);
    char arrow = getArrow(_keyInput);
    getColorById(bufor, (char)_playerID+'0');
    printf("Destroy %sall%s enemies or collect %s%d%s points to win. Use \x18\x19\x1a< to navigate\n", BRIGHT_RED, RESET, BRIGHT_RED, _serverWinScore,RESET);
    printf("Your number %s%d%s   Current direction: %s%c%s   Your score %s%d%s\n",bufor,_playerID,RESET, BRIGHT_YELLOW, arrow, RESET, YELLOW, _playerScore,RESET);
    printf(RESET);
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

void Client::setUp(ClientSetup* setup)
{
    std::ifstream configFile(CONFIG_FILE);

    if (!configFile) {
        printf("Config file '%s' not found ! \n", CONFIG_FILE);
        strcpy_s(setup->serverIP, _DEFAULT_IP);
        strcpy_s(setup->serverPort, _DEFAULT_PORT);
        printf("Default settings has used \n");
    }
    else {
        std::string row, key, value;
        while (std::getline(configFile, row)) {
            key = row.substr(0, row.find_first_of(SEPARATOR));
            value = row.substr(row.find_first_of(SEPARATOR) + 1);

            if (key == "serverIP") {
                strcpy_s(setup->serverIP, value.c_str());
            }
            if (key == "serverPort") {
                strcpy_s(setup->serverPort, value.c_str());
            }
        }
        configFile.close();
    }
}


Client::Client(ClientSetup *setup) :
    _setup(*setup),
    _isRunning(true), 
    _isConnected(false),
    _hasReceivedConnMsg(false)
{
    if (initConsole() != 0) {
        if (_fdwSaveOldInMode != 0)
            SetConsoleMode(_hStdin, _fdwSaveOldInMode);
        if (_fdwSaveOldOutMode != 0)
            SetConsoleMode(_hStdout, _fdwSaveOldOutMode);
        ExitProcess(1);
    }

    printf("Trying to connect to %s... \n", setup->serverIP);
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
    uint8_t mode = _recvbuf[0] & 0b11;
    switch (mode) {
    case Client::CONN:
    {
        char* msg = _recvbuf;
        short winscore = 0;
        _mapSizeX = _recvbuf[1];
        _mapSizeY = _recvbuf[2];
        _playerID = _recvbuf[0]>>2;
        __asm {
            push eax
            push ebx
            mov ebx, msg
            mov al, [ebx + 4]//weź młodszą częśc
            mov ah, [ebx + 5]//weź starszą część
            mov winscore, ax
            pop ebx
            pop eax
        }
        _serverWinScore = winscore;
        _hasReceivedConnMsg = true;
        printf("%sCONNECTED%s\n", BRIGHT_GREEN, RESET);
        break;
    }
    case Client::DISC:
    {
        int winnerID = _recvbuf[1];
        _isRunning = false;
        //odbierz id wygrywa i sprawdz czy to ty
        printf("Game has ended: ");
        if (_playerID == winnerID) {
            printf("%sYou won!\n%s", BRIGHT_GREEN, RESET);
        }
        else {
            printf("%sYou lost!\n%s", BRIGHT_RED, RESET);
        }
        //
        break;
    }
    case Client::SPECTATE:
        printf("You lost. Now watch\n");
    case Client::MAP:
    {
        char* msg = _recvbuf;
        short score = 0;
        __asm {
            push eax
            push ebx
            mov ebx, msg
            mov al, [ebx + 1]//weź młodszą częśc
            mov ah, [ebx + 2]//weź starszą część
            mov score, ax
            pop ebx
            pop eax
        }
        _playerScore = score;
        printGame(_recvbuf + 3);
        break;
    }
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
            return 0;
        }

        if (result > 0) {
            //int result = select(0, &readSet, nullptr, nullptr, &timeout);
            //if (result == SOCKET_ERROR) {
            //    printf("Line %d in function %s \t select failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
            //    client->_isRunning = false;
            //    return 0;
            //}
            iResult = recv(client->_socket, client->_recvbuf, client->_recvbuflen, 0);
            if (iResult > 0) {
                client->decodeMessage();
            }
            else if (iResult == 0) {
                printf("Connection closed\n");
                client->_isRunning = false;
                return 0;
            }
            else {
                //printf("Line %d in function %s \t recv failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
                printf("Game has ended: ");
                // silver tape
                if (client->_playerScore >= client->_serverWinScore) {
                    printf("%sYou won!\n%s", BRIGHT_GREEN, RESET);
                }
                else if (client->_playerScore <= client->_serverWinScore) {
                    printf("%sYou lost!\n%s", BRIGHT_RED, RESET);
                }
                else {
                    printf("\n");
                }
                // ! silver tape
                client->_isRunning = false;
                return 0;
            }
        }
        else if (result == 0 && !client->_hasReceivedConnMsg) {
            printf("%sServer busy%s\n", RED, RESET);
            client->_isRunning = false;
            return 0;
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
        if (WaitForSingleObject(client->_hStdin, 1000) != WAIT_OBJECT_0) {
            continue;
        }
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