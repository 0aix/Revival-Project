#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
	printf("WSASTARTUP OK\n");
	

	WSACleanup();
	getchar();
	return 0;
}