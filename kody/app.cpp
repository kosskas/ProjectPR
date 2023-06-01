#include "Client.h"

int __cdecl main(int argc, char** argv) {
    Client client("");
    int licz = 16;
    while (licz) {
        client.sendMessage("trrr",4);
        Sleep(1000);
        licz--;
    }
    client.sendMessage("END",4);
    return 0;
}