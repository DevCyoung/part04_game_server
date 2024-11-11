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
#include <queue>
#include "SocketUtils.h"

int main()
{
	SocketUTils::Init();

	SOCKET socket = SocketUTils::CreateSocket();

	SocketUTils::BindAnyAddress(socket, 7777);

	SocketUTils::Listen(socket);

	::accept(socket, nullptr, nullptr);

	std::cout << "Client Connected" << std::endl;


	while (1);

	SocketUTils::Clear();
}
