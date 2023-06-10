#include "General.h"
#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = MAX_NUMBER_OF_CLIENTS;
    setup.port = PORT;
    setup.mapSizeY = MAP_SIZE_Y;
    setup.mapSizeX = MAP_SIZE_X;

    Server server(setup);

    server.run();

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}