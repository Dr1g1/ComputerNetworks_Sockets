#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "w2_32.lib")

#define SERVER_PORT 8888
#define BUFF_SIZE 1024

using namespace std;

void printMessage(const string& message, int len)
{
    cout << "Poruka: ";
    for (int i = 0; i < len; i++)
        cout << message[i];
    cout << endl;
}

int main()
{
    // 1. Kreiranje i inicijalizacija WSAData strukture - info o verziji i implementaciji soketa i f-ja koje koristimo u ostatku koda
    WSAData wsa;
    WSAStartup(0x0202, &wsa);
    // 2. Kreiranje soketa
    SOCKET serverSock = socket(AF_INET, SOCK_DGRAM, 0);
    // 3. Kreiranje sockaddr_in objekta - info kojima se soket identifikuje
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    // 4. Bindovanje
    bind(serverSock, (sockaddr*)&server, sizeof(server));
    // 5. Kreira se sockaddr_in za klijenta - jer server slusa svuda, pa da bi znao gde da vrati eho mora da ima ovaj objekat koji ce da cuva info o tome kako da posalje bas tom klijentu
    sockaddr_in client;
    int len = sizeof(struct sockaddr_in);
    // 6. Komunikacija izmedju klijenta i servera(sendto i recvfrom)
    while (true)
    {
        char echoBuff[BUFF_SIZE];
        int recvMsgSize = recvfrom(serverSock, echoBuff, BUFF_SIZE, 0, (sockaddr*)&client, &len);
        printMessage(echoBuff, recvMsgSize);
        sendto(serverSock, echoBuff, recvMsgSize, 0, (sockaddr*)&client, len);
    }
    // 7. Zatvaranje i ciscenje
    closesocket(serverSock);
    WSACleanup();
    return 0;
}