#include "stdafx.h"
#include <stdio.h>

#define DEFAULT_PORT 17460

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	printf("WSASTARTUP OK\n");

	//TCP
	/*hostent* host = gethostbyname("localhost");

	SOCKET skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN sockaddr;
	sockaddr.sin_port = htons(17460);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	connect(skt, (SOCKADDR*)(&sockaddr), sizeof(SOCKADDR_IN));

	char buffer[256];
	recv(skt, buffer, 256, 0);
	printf(buffer);

	shutdown(skt, SD_BOTH);
	closesocket(skt);*/

	//UDP
	SOCKET skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(DEFAULT_PORT);

	int i = bind(skt, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR_IN));

	DWORD nonblocking = 1;
	ioctlsocket(skt, FIONBIO, &nonblocking);

	char buffer[256];
	SOCKADDR_IN addr;
	int length = sizeof(SOCKADDR_IN);
	while (true)
	{
		int i = recvfrom(skt, buffer, 256, 0, (SOCKADDR*)&addr, &length);
		if (i > 0)
			break;
		Sleep(10);
	}
	printf(buffer);

	shutdown(skt, SD_BOTH);
	closesocket(skt);

	WSACleanup();
	getchar();
	return 0;
}