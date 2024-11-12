#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
/*-----------------
* Session
-----------------*/
Session::Session()
{
	_socket = SocketUTils::CreateSocket();
}

Session::~Session()
{
	SocketUTils::Close(_socket);
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numofBytes)
{
	// TODO
}
