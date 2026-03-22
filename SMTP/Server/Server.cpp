#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFF_SIZE 1024

using namespace std;

void send(SOCKET clientSock, string s)//salje tekst klijentu
{
	//ispis sta ce da posalje i zatim ga salje klijentu
	cout << s << endl;
	send(clientSock, s.c_str(), s.length() + 1, 0);
}

string receive1(SOCKET clientSock)//prima poruku od klijenta i ispisuje je na konzolu
{
	//prima poruku i ispisuje sta je primio i vraca to sto je primio kao povratnu vrednost
	vector<char> echoBuff(BUFF_SIZE);
	recv(clientSock, echoBuff.data(), echoBuff.size(), 0);

	string s = echoBuff.data();
	cout << "Primio: " << s << endl;
	return s;
}

map<string, string> receive2(SOCKET clientSock, string tr)
//prima string, proverava da li pocinje ocekivanom komandom(tr), \
odvaja komandu od argumenta i vraca u map<string, string>
{
//poziv za primanje poruke a onda dodavanje elementa nekog u mapu
	string s = receive1(clientSock);

	string a = s.substr(0, tr.length());//izdvaja deo poruke koja predstavlja kod(npr HELO)
	//s = s.substr(0, tr.length() + 1);
	s = s.substr(tr.length() + 1);//uzima se tekst poruke nakon koda
	map<string, string> mapa;
	mapa[a] = s;//kljuc je kod poruke, a vrednost je tekst
	return mapa;
}

void sendCommand(SOCKET clientSock, string s)
{
//ptrazi unos od korisnika pa salje komandu zajedno sa argumentom
	string sParam = "";
	cout << "Unesite: ";
	getline(cin, sParam);
	s = s + " " + sParam;
	send(clientSock, s);
}

void sendCommand(SOCKET clientSock, string h, string a)
{
//prima komandu i argument i salje ih zajedno
	string s = h + " " + a;
	send(clientSock, s);
}

int main()
{
	WSAData wsa;
	WSAStartup(0x0202, &wsa);

	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(25);//port za SMTP

	bind(listenSock, (sockaddr*)&server, sizeof(server));

	listen(listenSock, 3);
	
	map<string, string> mainMap;//glavna mapa
	map<string, string> tmpMap;//pomocna mapa

	while (true)
	{
		//pravi se soket za konkretnog klijenta
		sockaddr_in client;
		SOCKET clientSock;
		int clen = sizeof(sockaddr_in);

		sendCommand(clientSock, "220");
		tmpMap = receive2(clientSock, "HELO");
		while (tmpMap["HELO"] == "") {
			send(clientSock, "Error");
			tmpMap = receive2(clientSock, "HELO");
		}
		mainMap["HELO"] = tmpMap["HELO"];
		string s = "Hello, " + mainMap["HELO"] + ", pleased to meet you";
		sendCommand(clientSock, "250", s);

		tmpMap = receive2(clientSock, "MAIL FROM: ");
		while (tmpMap["MAIL FROM: "] == "") {
			send(clientSock, "Error");
			tmpMap = receive2(clientSock, "MAIL FROM: ");
		}
		mainMap["MAIL FROM: "] = tmpMap["MAIL FROM: "];
		s = mainMap["MAIL FROM: "] + "Sender ok";
		sendCommand(clientSock, "250", s);

		tmpMap = receive2(clientSock, "RCPT TO: ");
		while (tmpMap["RCPT TO: "] == "") {
			send(clientSock, "Error");
			tmpMap = receive2(clientSock, "RCPT TO: ");
		}
		mainMap["RCPT TO: "] = tmpMap["RCPT TO: "];
		s = mainMap["RCPT TO: "] + "Receipient ok";
		sendCommand(clientSock, "250", s);

		if (receive1(clientSock) == "DATA") {
			s = "Enter mail, end with . on a line by itself";
			sendCommand(clientSock, "354", s);
		}

		string data = receive1(clientSock);
		s = "Message accepted for delivery";
		sendCommand(clientSock, "250", s);
		string q = receive1(clientSock);

		if (q == "QUIT")
		{
			sendCommand(clientSock, "221", "closing the connection");
			closesocket(clientSock);
			break;
		}
	}

	closesocket(listenSock);
	WSACleanup();

	return 0;
}