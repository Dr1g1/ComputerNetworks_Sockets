#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFF_SIZE 1024

using namespace std;

void sendCommand(SOCKET clientSock, const string& s)
{
	send(clientSock, s.c_str(), s.length() + 1, 0);
	cout << "Saljem: " << s << endl;
}

string receiveCommand(SOCKET clientSock)
{
	vector<char> echoBuff(BUFF_SIZE);
	recv(clientSock, echoBuff.data(), 1024, 0);
	string s = echoBuff.data();
	cout << "Primio: " << s << endl;
	return s;
}

bool sendCommand1(SOCKET clientSock, string sCommand)
{
	string sParametar = "";
	cout << "Unesite: " << sCommand << ": ";
	getline(cin, sParametar);
	sendCommand(clientSock, sCommand + " " + sParametar);

	string sRes = receiveCommand(clientSock);
	int nInd = sRes.find("+OK");
	return nInd >= 0;
}

int main()
{
	WSAData wsa;
	WSAStartup(0x0202, &wsa);
	
	SOCKET sock;

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(110);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (sockaddr*)&server, sizeof(server));

	while (true)
	{
		string sRes = receiveCommand(sock);
		int nInd = sRes.find("+OK");
		if (nInd >= 0)
		{
			while (!sendCommand1(sock, "user"))
			{
				cout << "Pogresno korisnicko ime, pokusajte ponovo." << endl;
			}
			while (!sendCommand1(sock, "pass"))
			{
				cout << "Pogresna lozinka, pokusajte ponovo." << endl;
			}
			cout << "Ulogovan si!" << endl;
		}
		else {
			//server ne radi - ne znam sta ovde da ispise - da li da ispise server ne radi?
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}