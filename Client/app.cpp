#include "Client.h"
#include <iostream>
int __cdecl main(int argc, char** argv) {
    
    //ip będzie w argv
    Client client(argv[1]);
    printf("\033[35;106m Hello, world!\033[m");
    client.start();
    
    return 0;
}