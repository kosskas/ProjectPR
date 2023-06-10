#include "General.h"
#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = 3;
    setup.port = PORT;
    setup.mapSizeY = 30;
    setup.mapSizeX = 30;

    Server server(setup);

    server.run();

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}