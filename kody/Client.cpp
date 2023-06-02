#include "Client.h"

Client::Client(const char* ipadress) {
    if (initWinsock(ipadress) != 0) {
        ExitProcess(1);
    }
    MsgReceiver = CreateThread(NULL, 0, &MsgReceiverListener, (void*)this, 0, MsgReceiverID);
    if (MsgReceiver == NULL) {
        printf("Failed to create client thread.\n");
        WSACleanup();
        ExitProcess(1);
    }
}



int Client::sendMessage(const char* sendbuf, int len) {
    // Send an initial buffer
    int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("Blad w linii: %d [send failed with error: %d]\n",__LINE__, WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Wyslano %ldB\n", iResult);
    return 0;
}

DWORD WINAPI MsgReceiverListener(LPVOID param) {
    Client client = *(Client*)param;
    int iResult;
    do {
        iResult = recv(client.ConnectSocket, client.recvbuf, client.recvbuflen, 0);
        if (iResult > 0) {
            printf("\nOtrzymano: %dB\t", iResult);
           printf("Wynik: %s\n", client.recvbuf);
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            return 0;
        }
        else {
            printf("Blad w linii: %d [recv failed with error: %d]\n",__LINE__, WSAGetLastError());
            return 1;
        }
    } while (true);
    return 0;
}

Client::~Client() {
    // cleanup
    CloseHandle(MsgSender);
    CloseHandle(MsgReceiver);
    int iResult = shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
    WSACleanup();
}

int Client::initWinsock(const char* ipadress) {
    // Initialize Winsock
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