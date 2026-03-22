#include <iostream>
#include <winsock2.h>
#include <map>
#include <vector>
#pragma comment(lib, "ws2_23.lib")

#define BUFF_SIZE 1024

using namespace std;

void sendCommand(SOCKET clientSock, const string &s)
{
	send(clientSock, s.c_str(), s.length() + 1, 0);
	cout << "Saljem: " << s << endl;
}

string receiveCommand(SOCKET clientSock)
{
	vector<char> echoBuff(BUFF_SIZE);
	recv(clientSock, echoBuff.data(), echoBuff.size(), 0);
	string s = echoBuff.data();
	cout << "Primio: " << s << endl;
	s = s.substr(s.find(' ') + 1);
	return s;
}

int main()
{
	map<string, string> mapUsernameAndPassword;
	mapUsernameAndPassword["Nikola"] = "Nikola";
	mapUsernameAndPassword["admin"] = "admin";

	WSAData wsa;
	WSAStartup(0x0202, &wsa);
	
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);//za protokol moze da se stavi IPPROTO_TCP

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(110);//110 je port koji koristi POP3
	server.sin_addr.s_addr = INADDR_ANY;
	
	bind(listenSock, (sockaddr*)&server, sizeof(server));
	listen(listenSock, 3);

	while (true)
	{
		sockaddr_in client;
		int len = sizeof(struct sockaddr_in);
		SOCKET clientSock = accept(listenSock, (sockaddr*)&client, &len);
		const string sOK = "+OK";
		const string sERR = "-ERR";

		sendCommand(clientSock, sOK);

		string sUsername = receiveCommand(clientSock);
//		while (mapUsernameAndPassword[sUsername] == "") -> nije dobro zato sto u isto vreme trazi value za kljuc koji ima vrednost koju je poslao klijent ali u isto vreme i pravi novi element
		while(sUsername.empty() || mapUsernameAndPassword.find(sUsername) == mapUsernameAndPassword.end())
		{
			sendCommand(clientSock, sERR);
			sUsername = receiveCommand(clientSock);
		}
		sendCommand(clientSock, sOK);

		string pass = receiveCommand(clientSock);
		while (pass.empty() || mapUsernameAndPassword[sUsername] != pass)
		{
			sendCommand(clientSock, sERR);
			pass = receiveCommand(clientSock);
		}
		sendCommand(clientSock, sOK);
		closesocket(clientSock);
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}