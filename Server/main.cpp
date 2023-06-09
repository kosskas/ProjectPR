#include "Server.h"


#define BACKLOG_LENGTH SOMAXCONN   // this is to long backlog queue !!!
#define MAX_NUMBER_OF_CLIENTS 2
#define BUFFER_SIZE 4096//512
#define PORT 27015


int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = MAX_NUMBER_OF_CLIENTS;
    setup.port = PORT;
    setup.mapSizeY = 40;
    setup.mapSizeX = 63;

    Server server(setup);

    server.run();

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}