#include "Client.h"

int __cdecl main(int argc, char** argv) {
    //ip będzie w argv
    Client client("");
    int licz = 10;
    while (licz) {
        client.sendMessage("SLOW",4);
        Sleep(1000);
        licz--;
    }
    client.sendMessage("END",4);
    return 0;
}