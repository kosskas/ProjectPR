#include "Server.h"


#define BACKLOG_LENGTH SOMAXCONN   // this is to long backlog queue !!!
#define MAX_NUMBER_OF_CLIENTS 3
#define BUFFER_SIZE 512
#define PORT 27015

int __cdecl main(void){
    ServerSetup setup;
    setup.backlog = BACKLOG_LENGTH;
    setup.bufferSize = BUFFER_SIZE;
    setup.maxNumberOfClients = MAX_NUMBER_OF_CLIENTS;
    setup.port = PORT;

    Server server(setup);

    server.run();

    server.~Server();

    char c = getchar();
    c = getchar();
    ExitProcess(0);
}