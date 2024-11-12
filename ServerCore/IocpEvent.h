#pragma once
#include "CorePch.h"

class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	Recv,
	Send,
};
//Virtual ����ȵ�. ù��° ���ڰ� �޶���
/*----------------
* IocpEvent
----------------*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }

protected:
	EventType _type;
};

/*----------------
* ConnectEvent
----------------*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*----------------
* AccepttEvent
----------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}
	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }
private:
	Session* _session = nullptr;
};

/*----------------
* RecvtEvent
----------------*/
class RecvtEvent : public IocpEvent
{
public:
	RecvtEvent() : IocpEvent(EventType::Recv) {}
};

/*----------------
* SendEvent
----------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};
