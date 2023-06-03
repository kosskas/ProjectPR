#include "Server.h"

#define compareString(a, b) strcmp(a, b) == 0

Server::Server() {
    if (initWinsock() != 0) {
        ExitProcess(1);
    }
}

Server::~Server() {
    // cleanup
    CloseHandle(SenderTh);
    CloseHandle(PingerTh);

    closesocket(ListenSocket);
    for (SOCKET* ClientSocket : ClientSockets) {
        closesocket(*ClientSocket);
    }
    WSACleanup();
}

void Server::start() {
    SenderTh = CreateThread(NULL, 0, &Broadcast, (void*)this, 0, senderThID);
    if (SenderTh == NULL) {
        printf("Line %d\t in function %s\tFailed to create thread.\n",__LINE__,__FUNCTION__);
        WSACleanup();
        ExitProcess(1);
    }
    PingerTh = CreateThread(NULL, 0, &Pinger, (void*)this, 0, pingerThID);
    if (PingerTh == NULL) {
        printf("Line %d\t in function %s\tFailed to create thread.\n", __LINE__, __FUNCTION__);
        WSACleanup();
        ExitProcess(1);
    }
    JoinerTh = CreateThread(NULL, 0, &NewClientJoiner, (void*)this, 0, joinerThID);
    if (JoinerTh == NULL) {
        printf("Line %d\t in function %s\tFailed to create thread.\n", __LINE__, __FUNCTION__);
        WSACleanup();
        ExitProcess(1);
    }
    while (true) {
        //aktywne oczekiwanie;
    }
}

DWORD __stdcall NewClientJoiner(LPVOID param) {
    Server* server = (Server*)param;
    while (true) {
        // Accept a client socket
        SOCKET* ClientSocket = new SOCKET;
        *ClientSocket = accept(server->ListenSocket, NULL, NULL);
        if (*ClientSocket == INVALID_SOCKET) {
            printf("Line %d in function %s\taccept failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
        }
        DWORD threadId;
        HANDLE clientThread = CreateThread(NULL, 0, &ClientListener, (void*)ClientSocket, 0, &threadId);
        if (clientThread == NULL) {
            printf("Failed to create thread.\n");
            closesocket(*ClientSocket);
            delete ClientSocket;
            ClientSocket = NULL;
        }
        else
            server->ClientSockets.push_back(ClientSocket);
    }
    return 0;
}

DWORD __stdcall ClientListener(LPVOID param) {
    SOCKET ClientSocket = *(SOCKET*)param;
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    int iSendResult = send(ClientSocket, "CONNECTED", strlen("CONNECTED"), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("Line %d in function %s\tsend failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
        closesocket(ClientSocket);
        return 0;
    }
    
    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("Recevied: %s", recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closing...\n");
        }
        else {
            printf("Line %d in function %s\recv failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
            closesocket(ClientSocket);
            param = NULL;
            return 0;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        delete param;
        param = NULL;
        return 1;
    }
    delete param;
    param = NULL;
    return 0;
}

DWORD __stdcall Broadcast(LPVOID param) {
    printf("ClientSender");
    Server *server = (Server*)param;
    while (true) {
        printf("\tbump %d\n", server->ClientSockets.size());
        Sleep(1000);
        for (SOCKET* ClientSocket : server->ClientSockets) {
                if (ClientSocket != NULL) {
                    char recvbuf[DEFAULT_BUFLEN] = { 0 };
                    int recvbuflen = DEFAULT_BUFLEN;
                    const char* msg = "PING"; ///TU BĘDZIE WYSYŁANA MAPA
                    int iSendResult = send(*ClientSocket, msg, strlen(msg), 0);
                    if (iSendResult == SOCKET_ERROR) {
                        printf("Line %d in function %s\tsend failed with error: %d\n", __LINE__, __FUNCTION__, WSAGetLastError());
                    }
                }
        }
    }
    return 0;
}

DWORD __stdcall Pinger(LPVOID param) {
    Server* server = (Server*)param;
    while (true) {
        Sleep(1334);
        server->ClientSockets.remove_if([](SOCKET* s) {
            return send(*s, NULL, 0, 0) == SOCKET_ERROR;
            });
        printf("Pinger\n");
    }
    return 0;
}

int Server::initWinsock() {
    // Initialize Winsock
    WSADATA wsaData;
    addrinfo* result = NULL;
    addrinfo hints;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    return 0; //return 0 if ok
}