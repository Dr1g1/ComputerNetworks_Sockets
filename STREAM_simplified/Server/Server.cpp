#include <iostream>
#include <string>
#include <vector>
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
    // 2. Kreiranje listen soketa - SOCKET struktura
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    // 3. Kreiranje sockaddr_in objekta
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    // 4. Bindovanje soketa i sockaddr_in strukture
    bind(listenSock, (sockaddr*)&server, sizeof(sockaddr));
    // 5. Osluskivanje
    listen(listenSock, 3);
    while (true)
    {
        // 6. Kreiranje posebnog soketa za obradu klijenta 
        SOCKET clientSock;
        sockaddr_in client;
        int sz = sizeof(struct sockaddr_in);
        // 7. Server prihvata klijenta koji je prvi naisao i njemu dodeljuje clientSock
        clientSock = accept(listenSock, (sockaddr*)&client, &sz);
           //accept fja vraca objekat SOCKET sa dodeljenim vrednostima atrubutima
        // 8. Komunikacija sa klijentom 
        char bafer[BUFF_SIZE];
        int rcvMsgSz = recv(clientSock, bafer, BUFF_SIZE, 0);
        printMsg(bafer, rcvMsgSz);

        while (rcvMsgSz > 0)
        {
            send(clientSock, bafer, rcvMsgSz, 0);
            rcvMsgSz = recv(clientSock, bafer, rcvMsgSz, 0);
            printMsg(bafer, rcvMsgSz);
        }
        closesocket(clientSock);
    }
    // 9. Zatvaranje soketa i ciscenje 
    closesocket(listenSock);
    WSACleanup();
    return 0;
}