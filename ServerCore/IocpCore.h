#pragma once
#include "CorePch.h"
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

//like session
/*------------------
* IocpObject
--------------------*/
class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numofBytes = 0) abstract;
};


/*------------------
* IocpCore
--------------------*/
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }

	bool Register(class IocpObject* iocpObject);
	bool Dispatch(uint32 timeoutMS = INFINITE);;

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;
