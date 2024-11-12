#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include <atomic>

/*-----------------
* Session
-----------------*/
class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }

public:
public:
	// TEMP
	char _recvBuffer[1000];
	// IocpObject을(를) 통해 상속됨
	HANDLE GetHandle() override;
	void Dispatch(IocpEvent* iocpEvent, int32 numofBytes) override;

private:
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	std::atomic<bool> _connected = false;
};
