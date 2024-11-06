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


	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (INVALID_SOCKET == listenSocket)
	{
		std::cout << "socket error" << std::endl;
	}


	u_long on = 1;
	if (SOCKET_ERROR == ::ioctlsocket(listenSocket, FIONBIO, &on))
	{
		std::cout << "ioctlsocket error" << std::endl;
		return 0;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7878);
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	if (SOCKET_ERROR == ::bind(listenSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr.sin_addr)))
	{
		std::cout << "bind error" << std::endl;
		return 0;
	}

	if (SOCKET_ERROR == ::listen(listenSocket, SOMAXCONN))
	{
		return 0;
	}

	std::cout << "Accept" << std::endl;

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);


	
	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			//문제가 있을수도 없을수도있음 뒤끝작렬 너가 넌 블러킹으로 하라며!
			//아직 완료가 되지않았다던가...
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}


			// Error
			break;
		}

		std::cout << "Client Connected!" << std::endl;

		// Recv
		while (true)
		{
			char recvBuffer[1000];
			int recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				//문제가 있을수도 없을수도있음 뒤끝작렬 너가 넌 블러킹으로 하라며!
				//아직 완료가 되지않았다던가...
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
			}
			else if (recvLen == 0)
			{
				// 연결 끊김
				break;
			}

			std::cout << "Recv Data Len = " << recvLen << std::endl;

			// Send
			while (true)
			{
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					//문제가 있을수도 없을수도있음 뒤끝작렬 너가 넌 블러킹으로 하라며!
					//아직 완료가 되지않았다던가...
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
					{
						continue;
					}

					break;
				}
			}

			std::cout << "Send Data ! Len = " << recvLen << std::endl;


		}
	}	


	while (true);

	::WSACleanup();
}