//#include "pch.h"
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
	service.sin_port = htons(7878); //host to network short 엔디안 이슈
	
	if (SOCKET_ERROR == ::bind(socket, (const sockaddr*)&service, sizeof(service)))
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : Connect Fail" << std::endl;
		return 0;
	}

	//other
	SOCKADDR_IN other;
	::memset(&other, 0, sizeof(SOCKADDR_IN));
	other.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &other.sin_addr);
	other.sin_port = htons(7899); //host to network short 엔디안 이슈

	int pa[1000];

	int& a = pa[0];
	a = 0;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//ConnectedUDP
	::connect(socket, (sockaddr*)&other, sizeof(other));

	while (1)
	{
		//ConnectedUDP
		int sendlen = ::send(socket, (const char*)pa, sizeof(pa), 0);
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//std::cout << sendlen << " s " << a  <<std::endl;
		
		if (sendlen == -1)
		{
			std::cout << "Error" << std::endl;
			break;
		}

		if (a >= 1000000)
		{
			int s = ::sendto(socket, (const char*)&pa, 0, 0, (sockaddr*)&other, sizeof(other));
				
			//std::cout << "sendto0" << std::endl;
			break;
		}

		++a;
	}


	::sendto(socket, (const char*)&pa, 0, 0, (sockaddr*)&other, sizeof(other));

	std::cout << a << std::endl;

	

	while (true);


	::WSACleanup();
}
