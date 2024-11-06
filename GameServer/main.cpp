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
	//Winsock lib �ʱ�ȭ
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
			// ���� ��� �߾�� �ߴµ� �ʰ� ����ŷ���� �϶��
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			// �̹� ����� ���¶�� beak	
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
			//������ �������� ������������ �ڳ��۷� �ʰ� �� ��ŷ���� �϶��!
			//���� �Ϸᰡ �����ʾҴٴ���...
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
