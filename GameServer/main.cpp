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

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		int errCode = ::WSAGetLastError();
		std::cout << errCode << " : INVALID_SOCKET" << std::endl;
		return 0;
	}

	u_long on = 1;
	if (SOCKET_ERROR == ::ioctlsocket(clientSocket, FIONBIO, &on))
	{
		std::cout << "ioctlsocket error" << std::endl;
		return 0;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7878);
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	while (true)
	{
		if (SOCKET_ERROR == ::connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)))
		{
			// 원래 블록 했어야 했는데 너가 논블로킹으로 하라며
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			// 이미 연결된 상태라면 beak	
			if (::WSAGetLastError() == WSAEISCONN)
			{
				break;
			}

			break;
		}
	}
	
	std::cout << "Connected to Server! " << std::endl;

	
	char sendBuffer[100] = "Hello World!";
	// Send
	while (true)
	{
		if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			//문제가 있을수도 없을수도있음 뒤끝작렬 너가 넌 블러킹으로 하라며!
			//아직 완료가 되지않았다던가...
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}


			//error
			break;	
		}
	}



	while (true);


	::WSACleanup();
}
