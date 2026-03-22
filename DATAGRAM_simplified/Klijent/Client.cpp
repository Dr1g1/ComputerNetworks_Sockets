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
    //1. WSAData kreiranje i inicijalizacija - informacije o verziji i implementaciji soketa i f-ja
    WSAData wsa;
    WSAStartup(0x0202, &wsa);
    //2. Kreiranje soketa
    SOCKET clientSock = socket(AF_INET, SOCK_DGRAM, 0);
    //3. Kreiranje objekta sockaddr_in
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    //ne bindujemo!!! - nece da radi ako bindujemo jer ce klijent onda da salje sam sebi prakticno
    //4. Komunikacija izmedju klijenta i servera
    string echoBuff;
    int sz = sizeof(struct sockaddr_in);
    while (true)
    {
        getline(cin, echoBuff);
        sendto(clientSock, echoBuff.c_str(), (int)echoBuff.length(), 0, (sockaddr*)&server, sz);
        int recvMsgSize = 0;
        char buff[BUFF_SIZE];
        recvMsgSize = recvfrom(clientSock, buff, BUFF_SIZE, 0, (sockaddr*)&server, &sz);
        printMessage(buff, recvMsgSize);
    }
    //5. Zatvaranje soketa i ciscenje
    closesocket(clientSock);
    WSACleanup();
    return 0;
}
