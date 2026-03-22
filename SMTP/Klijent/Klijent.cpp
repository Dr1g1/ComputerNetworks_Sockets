#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib");

#define BUFF_SIZE 1024

using namespace std;

void send(SOCKET clientSock, string s)
{//prikaz poruke na terminal i slanje serveru
	cout << s << endl;
	send(clientSock, s.c_str(), s.length(), 0);
}

string receive(SOCKET clientSock)
{
	vector<char> echoBuff(BUFF_SIZE);
	recv(clientSock, echoBuff.data(), echoBuff.size(), 0);

	string s = echoBuff.data();
	cout << "Primio: " << s << endl;
	s = s.substr(0, 3);//odvoji kod od poruke i upise kod u s koji vraca
	return s;
}

void sendCommand(SOCKET clientSock, string s)
{
	string sParam = "";
	cout << "Unesite: " << endl;
	getline(cin, sParam);
	s = s + " " + sParam;
	send(clientSock, s.c_str(), s.length(), 0);
}

int main()
{
	WSAData wsa;
	WSAStartup(0x0202, &wsa);

	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(25);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(clientSock, (sockaddr*)&server, sizeof(server));

	while (true)
	{
		string s = receive(clientSock);//prima 220
		while (s != "220")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}

		sendCommand(clientSock, "HELO");//salje HELO
		
		s = receive(clientSock);
		while (s != "250")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}

		sendCommand(clientSock, "MAIL FROM: ");

		s = receive(clientSock);
		while (s != "250")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}

		sendCommand(clientSock, "RCPT TO: ");

		s = receive(clientSock);
		while (s != "250")
		{
			sendCommand(clientSock, "Error");
			s = receive(clientSock);
		}

		send(clientSock, "DATA");
		s = receive(clientSock);//prima 354
		while (s != "354")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}

		char data[BUFF_SIZE];
		strcpy(data, "");
		char tmp[100];
		char provera[5];
		do
		{
			fgets(tmp, 100, stdin);
			strcat(data, tmp);
			int l = strlen(data) - 3;
			int j = 0;
			for (int i = l; i <= strlen(data); i++)
				provera[j++] = data[i];

		} while (strcmp(provera, "\n.\n"));

		send(clientSock, data);

		s = receive(clientSock);
		while (s != "250")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}
		
		send(clientSock, "QUIT");

		s = receive(clientSock);
		while (s != "221")
		{
			send(clientSock, "Error");
			s = receive(clientSock);
		}
		break;

	}

	closesocket(clientSock);
	WSACleanup();

	return 0;
}