#include "Client.h"

int __cdecl main(int argc, char** argv) {
    Client client;
    int licz = 16;
    while (licz) {
        client.sendMessage("trrr");
        Sleep(1000);
        licz--;
    }
    client.sendMessage("END");
    return 0;
}