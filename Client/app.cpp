#include "Client.h"

int __cdecl main(int argc, char** argv)
{
    ClientSetup setup;
    Client::setUp(&setup);

    Client *client = new Client(setup);
    client->run();
    delete client;

    printf("\n Press Enter twice... \n");
    char c = getchar();
    c = getchar();

    ExitProcess(0);
}