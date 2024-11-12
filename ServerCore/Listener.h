#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;
class IocpEvent;
/*------------------
* Listener
------------------*/
class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/*외부에서 사용*/
	bool StartAccept(NetAddress netAddress);
	void CloseSocket();

	HANDLE GetHandle() override;
	void Dispatch(IocpEvent* iocpEvent, int32 numofBytes) override;

private:
	/*수신 관련*/
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);


protected:
	SOCKET _socket = INVALID_SOCKET;
	std::vector<AcceptEvent*> _acceptEvents;
};

