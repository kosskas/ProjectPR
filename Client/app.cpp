#include "Client.h"


#include <iostream>


int __cdecl main(int argc, char** argv)
{
    system("cls");

    ClientSetup setup;
    setup.serverIP = argv[1];
    setup.serverPort = DEFAULT_PORT;
    setup.bufferSize = DEFAULT_BUFLEN;
    setup.mapSizeX = MAP_SIZE_X;
    setup.mapSizeY = MAP_SIZE_Y;

    Client client(setup);

    client.run();

    client.~Client();

    printf("\n Press Enter twice... \n");
    char c = getchar();
    c = getchar();

    ExitProcess(0);
}

//wprintf(L"\033[35;106m Hello, world!\033[m");