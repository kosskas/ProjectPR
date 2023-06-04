#include "Server.h"


#define BACKLOG_LENGTH SOMAXCONN   // this is to long backlog queue !!!
#define MAX_NUMBER_OF_CLIENTS 2
#define BUFFER_SIZE 512
#define PORT 27015


int __cdecl main(void)
{
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = MAX_NUMBER_OF_CLIENTS;
    setup.port = PORT;

    Server server(setup);

    server.garbageCollector();
    server.waitForGamers();

    server.sendMap(); // !!! To wg dokumentacji ma byæ w klasie Gra !!!

    const unsigned int n = 1; // n - liczba pó³minut, przez które serwer dzia³a
    while (true) {
        Sleep(1000 * 30 * n);
        break;
    }

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}