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
#include "Listener.h"
#include "ThreadManager.h"
#include "SocketUtils.h"

ThreadManager tm;


int main()
{
	SocketUTils::Init();

	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; ++i)
	{
		tm.Launch([=]() {
			while (true)
			{
				GIocpCore.Dispatch();
			}
			});
	}

	tm.Join();

	SocketUTils::Clear();
}
