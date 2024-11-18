#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Memory.h"
#include "Session.h"
/*------------------
* Listener
------------------*/
Listener::~Listener()
{
	SocketUTils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		xdelete(acceptEvent);
	}

}

bool Listener::StartAccept(NetAddress netAddress)
{
	_socket = SocketUTils::CreateSocket();
	if (_socket == INVALID_SOCKET)
	{
		return false;
	}

	if (GIocpCore.Register(this) == false)
	{
		return false;
	}

	if (SocketUTils::SetReuseAddress(_socket, true) == false)
	{
		return false;
	}

	if (SocketUTils::Bind(_socket, netAddress) == false)
	{
		return false;
	}

	if (SocketUTils::Listen(_socket) == false)
	{
		return false;
	}

	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; ++i)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
	

	return true;
}

void Listener::CloseSocket()
{
	SocketUTils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numofBytes)
{
	//iocpEvent->GetType();

	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = make_shared< Session>();// xnew<Session>();

	acceptEvent->Init();
	acceptEvent->_session = session;
	
	DWORD byteRecived = 0;
	if (false == SocketUTils::AcceptEx(
		_socket,
		session->GetSocket(),
		session->_recvBuffer, 0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT & byteRecived,
		static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) // 문제가 일어난 상황
		{
			// 일단 다시 Accept 걸어준다.
			RegisterAccept(acceptEvent); // 낚시대를 던지는 행위
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->_session;

	if (false == SocketUTils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent); // 항상 낚시대를 끌어올린 다음에는 해야함
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == getpeername(session->GetSocket(), 
		OUT reinterpret_cast<SOCKADDR*>(&sockAddress),
		&sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));

	std::cout << "Client Connected" << std::endl;

	// 평생 재사용
	RegisterAccept(acceptEvent);
}
