#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <ServerHelper.h>
const int BUFSIZE = 1000;
#include <thread>
#include <chrono>
SOCKET CreateNonBlockListenSocket(const char* IP, const int port)
{
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, NULL);
	if (INVALID_SOCKET == listenSocket)
	{
		std::cout << "error" << std::endl;
	}

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on))
	{
		std::cout << "error" << std::endl;
	}

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	::inet_pton(AF_INET, IP, &sockAddr.sin_addr);

	if (SOCKET_ERROR == ::bind(listenSocket, (const sockaddr*)&sockAddr, sizeof(sockaddr)))
	{
		std::cout << "error" << std::endl;
	}

	if (SOCKET_ERROR == ::listen(listenSocket, SOMAXCONN))
	{
		std::cout << "error" << std::endl;
	}

	return listenSocket;
}

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int recvBytes = 0;
	int sendBytes = 0;
};

int main()
{
	//Winsock lib ÃÊ±âÈ­
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = ::WSAStartup(wVersionRequested, &wsaData);


	ServerHelper helper;

	SOCKET listenSocket = CreateNonBlockListenSocket("127.0.0.1", 7878);

	YSession session;

	session.bConnect = false;
	session.Socket = listenSocket;
	session.Symbol = "Lidar";
	session.IOType = eYSIO::SEND;

	helper.AddLitener(session);

	helper.StartThread();

	while (true)
	{
		char buffer[] = "Hello World!";
		int ret = helper.Write("Lidar", buffer, sizeof(buffer));

		helper.Update();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	::WSACleanup();
}
