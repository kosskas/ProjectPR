#include "Client.h"

int __cdecl main(int argc, char** argv)
{
    //system("cls");
    ClientSetup setup;
    setup.serverIP = argv[1];
    setup.serverPort = DEFAULT_PORT;
    setup.bufferSize = DEFAULT_BUFLEN;

    Client *client = new Client(setup);

    client->run();
    
    delete client;

    printf("\n Press Enter twice... \n");
    char c = getchar();
    c = getchar();

    ExitProcess(0);
}