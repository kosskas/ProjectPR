#include "General.h"
#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = 1;
    setup.port = PORT;
    setup.mapSizeY = 20;
    setup.mapSizeX = 60;

    Server *server = new Server(setup);

    server->run();

    delete server;

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}