#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

//-------------
//	Listener
//-------------

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		// TODO
		
		Xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	if (_service->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	// 소켓 옵션 설정
	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	// 클라이언트가 운좋게 바로 접속하면 바로 완료될 수도 있고
	// 아니면 나중에 iocp를 통해 워커 스레드들이 관찰하다가 완료될 수도 있음
	// 1개만 걸어주게 되면 접속자가 몰렸을 때 몇 명은 접속하지 못할 수 있음
	// 그래서 여유분을 두고 이벤트를 걸어줘야 됨
	// 지금은 임시로 여기 만들어 놓음
	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = Xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfByte)
{
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);

	// 현재 실질적으로 넣어준 이벤트가 Accept밖에 없음
	// 나중에 이벤트 별로 만들어야 되나?
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);

	// 실행
	ProcessAccept(acceptEvent);
}

// iocp에서 이벤트를 처리할 수 있도록 일감을 호출하고 예약하는 느낌
// Register는 미끼를 끼워넣는 단계 (정말 새로운 세션을 만드는 것)
// Process는 낚시대를 회수해서 물고기를 손질하는 단계(세션을 매니저에 처리, 컨텐츠 관련 부분)

// listener가 AcceptExtened를 호출하는 것이 핵심
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	// 클라가 접속시 관련된 모든 정보를 세션에 다 저장
	// 세션 풀을 만들어 사용해도 됨
	SessionRef session = _service->CreateSession(); //register IOCP

	acceptEvent->Init();
	acceptEvent->session = session;

	// acceptEvent에 session정보 연동
	// 그래야 나중에 Dispatch해서 이벤트를 가져왔을 때 어떤 세션을 넘겨줬는지 알 수 있음

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING/* 접속한 클라가 없어서 그냥 빠져나온 상황 */)
		{
			// 일단 다시 accept 걸어줌
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	// 어느 세션에서 이벤트가 일어난건데?
	SessionRef session = acceptEvent->session;


	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		// 낚시대를 끌어올렸으면 물고기가 잡히든 말든 미끼를 다시 꽂아야됨!!
		RegisterAccept(acceptEvent);

		return;
	}

	// 정보 추출, 로그 찍기(누가 접속했는지
	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);

		return;
	}

	// 세션에 접속한 클라이언트의 정보를 추출할 수 있다
	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();

	// TODO
	// 세션 매니저에 등록한다던가 등

	RegisterAccept(acceptEvent);
	// acceptEvent를 처음에 만들고 계속 재사용
}
