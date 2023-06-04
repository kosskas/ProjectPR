#include "Client.h"

Client::Client(const char* ipadress) {
    if (InitWinsock(ipadress) != 0) {
        WSACleanup();
        ExitProcess(1);
    }
    if (InitConsole() != 0) {
        if (fdwSaveOldMode != 0)
            SetConsoleMode(hStdin, fdwSaveOldMode);
        ExitProcess(1);
    }
    if (InitThreads() != 0) {
        ExitProcess(1);
    }
}

void Client::start() {
    int licz = 25;
    while (licz) {
        //printf("INPUT {%c}\t", keyInput);
        const char* temp = (const char*)&keyInput;
        sendMessage(temp, 1);
        Sleep(1000);
        licz--;
    }
    sendMessage("END", 4);
}

int Client::sendMessage(const char* sendbuf, int len) {
    // Send an initial buffer
    int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("Line %d in function %s\tsend failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Wyslano %ldB\n", iResult);
    return 0;
}

DWORD __stdcall MsgReceiverListener(LPVOID param) {
    Client *client = (Client*)param;
    int iResult;
    do {
        iResult = recv(client->ConnectSocket, client->recvbuf, client->recvbuflen, 0);
        if (iResult > 0) {
            printf("\nOtrzymano: %dB\t", iResult);
           printf("Wynik: %s\n", client->recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            return 0;
        }
        else {
            printf("Line %d in function %s \t recv failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
            return 1;
        }
    } while (true);
    return 0;
}

DWORD __stdcall KeyEventListener(LPVOID param) {
    Client *client = (Client*)param;
    DWORD cNumRead;
    INPUT_RECORD irInBuf[128];
    while (true) {
        if (!ReadConsoleInput(
            client->hStdin,      // input buffer handle 
            irInBuf,     // buffer to read into 
            128,         // size of read buffer 
            &cNumRead)) { // number of records read 
            printf("ReadConsoleInput");
            return 1;
        }
        for (int i = 0; i < cNumRead; i++) {
            if (irInBuf[i].EventType == KEY_EVENT) {
                client->keyInput = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;
            }
            if (irInBuf[i].EventType == KEY_EVENT && irInBuf[i].Event.KeyEvent.bKeyDown) {
                switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode) {
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
                default:
                    break;
                }
            }
        }
    }
    return 0;
}

Client::~Client() {
    // cleanup
   // CloseHandle(MsgSender);
    SetConsoleMode(hStdin, fdwSaveOldMode);
    CloseHandle(MsgReceiver);
    int iResult = shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
    WSACleanup();
}

int Client::InitWinsock(const char* ipadress) {
    // Initialize Winsock
    WSADATA wsaData;
    addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("192.168.56.1", DEFAULT_PORT, &hints, &result); //podstawić ipadress
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return 0;
}

int Client::InitConsole() {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        printf("GetStdHandle err");
        return 1;
    }
    // Save the current input mode, to be restored on exit. 
    if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
        printf("GetConsoleMode err");
        SetConsoleMode(hStdin, fdwSaveOldMode);
        return 1;
    }
    fdwMode = ENABLE_WINDOW_INPUT | ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS;
    if (!SetConsoleMode(hStdin, fdwMode)){
        printf("SetConsoleMode err");
        SetConsoleMode(hStdin, fdwSaveOldMode);
        return 1;
    }
    
    return 0;
}

int Client::InitThreads() {
    MsgReceiver = CreateThread(NULL, 0, &MsgReceiverListener, (void*)this, 0, MsgReceiverID);
    if (MsgReceiver == NULL) {
        printf("Failed to create MsgReceiverListener thread.\n");
        return 1;
    }
    keyEventListener = CreateThread(NULL, 0, &KeyEventListener, (void*)this, 0, keyEventListenerID);
    if (keyEventListener == NULL) {
        printf("Failed to create KeyEventListener thread.\n");
        return 1;
    }
    return 0;
}
