#include "General.h"
#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = 1;
    setup.port = PORT;
    setup.mapSizeY = MAP_SIZE_Y;
    setup.mapSizeX = MAP_SIZE_X;

    Server* server = new Server(setup);

    server->run();

    delete server;

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}