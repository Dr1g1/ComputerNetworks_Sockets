#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888
#define BUFF_SIZE 1024

using namespace std;

void printMsg(const string& msg, int len)
{
    cout << "Server: ";
    for (int i = 0; i < len; i++)
        cout << msg[i];
    cout << endl;
}

int main()
{
    // 1. WSAData kreiranje i inicijalizacija
    WSAData wsa;
    WSAStartup(0x0202, &wsa);
    // 2. Kreiranje soketa - SOCKET struktura
    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
    // 3. Kreiranje sockaddr_in objekta
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 4. Povezivanje sa serverom
    connect(clientSock, (sockaddr*)&server, sizeof(server));
    // 5. Komunikacija izmedju klijenta i servera
    while (true)
    {
        string bafer;
        getline(cin, bafer);
        send(clientSock, bafer.c_str(), bafer.length(), 0);

        char baff[BUFF_SIZE];
        int rcvMsgSz;
        do {
            (rcvMsgSz = recv(clientSock, baff, BUFF_SIZE, 0));
            printMsg(baff, rcvMsgSz);
        } while (rcvMsgSz == BUFF_SIZE);
    }
    // 6. Zatvaranje soketa i ciscenje
    closesocket(clientSock);
    WSACleanup();
    return 0;
}