#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, 0 byte recv
	Recv,
	Send
};

//--------------
//	IocpEvent
//--------------

// 오프셋 0번에 OVERLAPPED가 있으므로
// IocpEvent 포인터나 OVERLAPPED 포인터나 상관 없음
class IocpEvent : public OVERLAPPED
{
public:
	// virtual 사용 X, 0번 메모리에 다른 값이 들어갈 수 있음
	IocpEvent(EventType type);

	void		Init();

public:
	EventType	eventType;
	IocpObjectRef owner;
};

//-----------------
//	ConnectEvent
//-----------------

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

//-------------------
//	DisconnectEvent
//-------------------

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

//---------------
//	AcceptEvent
//---------------

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	// Accept할 때 추가적으로 필요한 인자가 있을 수 있음
	// 클라이언트 세션
	SessionRef	session = nullptr;
};

//--------------
//	RecvEvent
//--------------

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

//--------------
//	SendEvent
//--------------

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	// TEMP
	vector<BYTE> buffer;
};