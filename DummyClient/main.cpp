#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	//Winsock lib 초기화
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = ::WSAStartup(wVersionRequested, &wsaData);

	SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, NULL);
	if (socket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : INVALID_SOCKET" << std::endl;
		return 0;
	}

	SOCKADDR_IN service;
	::memset(&service, 0, sizeof(SOCKADDR_IN));
	service.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(7899); //host to network short 엔디안 이슈

	if (SOCKET_ERROR == ::bind(socket, (const sockaddr*)&service, sizeof(service)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	SOCKADDR_IN other;
	int tolen = sizeof(other);
	std::vector<int> v;

	while (1)
	{
		int recv[10000];
		int readlen = ::recvfrom(socket, (char*)&recv, sizeof(recv), 0, (sockaddr*)&other, &tolen);

		if (readlen == 0)
		{
			std::cout << "break" << std::endl;
			break;
		}

		v.push_back(recv[0]);

		//std::cout << readlen << " r " << recv[0] << " " << v.size() * sizeof(int) << std::endl;
	}

	

	int def = 0;
	for (int i = 1; i < v.size(); ++i)
	{
		if (v[i] - v[i - 1] != 1)
		{
			int v1 = v[i];
			int v0 = v[i - 1];
			++def;
			continue;
		}
	}

	std::cout << "Def " << def << std::endl;

	while (true);

	::WSACleanup();
}