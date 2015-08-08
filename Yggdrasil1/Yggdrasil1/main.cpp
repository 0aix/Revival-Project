#include "stdafx.h"
#include "Network.h"
#include <stdio.h>

#define SERVER_PORT 17460

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	printf("WSASTARTUP OK\n");

	//
	Socket socket;
	socket.Open(SERVER_PORT);

	Address to = Address(127, 0, 0, 1, SERVER_PORT);
	char str[] = "hey what's up";
	Packet packet;
	packet.data = str;
	packet.size = strlen(str);
	socket.Send(to, packet);

	socket.Close();



	//UDP
	/*SOCKET skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(SERVER_PORT);

	int i = bind(skt, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR_IN));
	
	DWORD nonblocking = 1;
	i = ioctlsocket(skt, FIONBIO, &nonblocking);

	char str[] = "hey what's up";
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(127 << 24 | 1); //127.0.0.1
	addr.sin_port = htons(SERVER_PORT);
	i = sendto(skt, str, strlen(str), 0, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));

	shutdown(skt, SD_BOTH);
	closesocket(skt);*/

	WSACleanup();
	return 0;
}