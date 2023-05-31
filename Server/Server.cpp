#include "Server.h"
using namespace std;
#define compareString(a, b) strcmp(a, b) == 0


Server::Server() {
    if (initWinsock() != 0) {
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
}

void Server::start() {

    while (true) {
        // Accept a client socket
        SOCKET* ClientSocket = new SOCKET;
        *ClientSocket = accept(ListenSocket, NULL, NULL);
        if (*ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            exit(1);
        }
        DWORD threadId;
        HANDLE clientThread = CreateThread(NULL, 0, &ClientListener, ClientSocket, 0, &threadId);
        if (clientThread == NULL) {
            printf("Failed to create client thread.\n");
            closesocket(*ClientSocket);
            delete ClientSocket;
        }
        else {
            Clients[online++] = clientThread;
        }
        
    }
    


    // No longer need server socket
    closesocket(ListenSocket);
}

DWORD WINAPI ClientListener(LPVOID param) {
    SOCKET ClientSocket = *(SOCKET*)param;
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    //int iSendResult = send(ClientSocket, "CONNECTED", strlen("CONNECTED"), 0);
    //if (iSendResult == SOCKET_ERROR) {
    //    printf("send failed with error: %d\n", WSAGetLastError());
    //    closesocket(ClientSocket);
    //    WSACleanup();
    //    return 0;
    //}
             
    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            printf("Recevied: %s", recvbuf);

            //// Echo the buffer back to the sender
            //iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            //if (iSendResult == SOCKET_ERROR) {
            //    printf("send failed with error: %d\n", WSAGetLastError());
            //    closesocket(ClientSocket);
            //    WSACleanup();
            //    return 1;
            //}
            //printf("Bytes sent: %d\n", iSendResult);
           /* if (compareString(recvbuf, "END")) {
                break;
            }*/
        }
        else if (iResult == 0) {
            printf("Connection closing...\n");
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 0;
        }

    } while (iResult > 0);

        // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    delete param;
}

int Server::initWinsock() {
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
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