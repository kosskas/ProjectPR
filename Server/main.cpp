#include "General.h"
#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = 1;
    setup.port = PORT;
    setup.mapSizeY = 10;
    setup.mapSizeX = 10;

    Server server(setup);

    server.run();

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}