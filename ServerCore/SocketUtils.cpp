#include "pch.h"
#include "SocketUtils.h"

/*---------------------------
* SocketUtils
---------------------------*/
LPFN_CONNECTEX SocketUTils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUTils::DisConnectEx = nullptr;
LPFN_ACCEPTEX SocketUTils::AcceptEx = nullptr;


void SocketUTils::Init()
{
	WSADATA wsadata;
	::WSAStartup(MAKEWORD(2, 2), OUT &wsadata);

	// 런타임에 주소 얻어오는 API
	SOCKET dummySocket = CreateSocket();
	bool ret = BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx));
	ret = BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisConnectEx));
	ret = BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx));
	Close(dummySocket);
}

void SocketUTils::Clear()
{
	WSACleanup();
}

bool SocketUTils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;	
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);

}

SOCKET SocketUTils::CreateSocket()
{
	//::socket은 WSA_FLAG_OVERLAPPED가 기본설정임	

	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUTils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;

	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUTils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUTils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUTils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUTils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket의 특성을 그대로 사용
bool SocketUTils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUTils::Bind(SOCKET socket, NetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(socket, (sockaddr*)&netAddr.GetSockAddr(), sizeof(SOCKADDR_IN));
}

bool SocketUTils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN mytAddress;
	mytAddress.sin_family = AF_INET;
	mytAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	mytAddress.sin_port = htons(port);

	return SOCKET_ERROR != ::bind(socket, (sockaddr*)&mytAddress, sizeof(mytAddress));
}

bool SocketUTils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUTils::Close(SOCKET& socket)
{
	::closesocket(socket);
	socket = INVALID_SOCKET;
}
