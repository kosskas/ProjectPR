#include "Server.h"

int __cdecl main(void)
{
    ServerSetup setup;
    Server::setUp(&setup);

    Server *server = new Server(setup);
    server->run();
    delete server;

    printf("Enter 2x..");
    char c = getchar();
    c = getchar();

    ExitProcess(0);
}