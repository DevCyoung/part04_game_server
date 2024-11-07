
//#include "pch.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <ServerHelper.h>

#include <thread>
#include <chrono>


int main()
{
	//Winsock lib √ ±‚»≠
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = ::WSAStartup(wVersionRequested, &wsaData);

	SOCKET socket = ::socket(AF_INET, SOCK_STREAM, NULL);

	u_long on = 1;
	if (::ioctlsocket(socket, FIONBIO, &on))
	{
		std::cout << "error" << std::endl;
	}
	//::connect(socket, (sockaddr*) & sockAddr, sizeof(sockAddr));

	ServerHelper helper;


	YSession session;

	session.bConnect = false;
	session.Socket = socket;
	session.OtherPort = 7878;
	session.OtherIP = "127.0.0.1";
	session.Symbol = "Lidar";
	session.IOType = eYSIO::RECV;

	helper.AddClient(session);
	helper.StartThread();


	while (true)
	{
		char buffer[256];
		int ret = helper.Read("Lidar", buffer, sizeof(buffer));

		if (ret > 0)
		{
			std::cout << buffer << std::endl;
		}

		helper.Update();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	::WSACleanup();
}
