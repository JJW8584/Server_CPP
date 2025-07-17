#include "pch.h"
#include "CorePch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include <chrono>
#include "ServerPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"

int main()
{

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	char sendData1[] = "가";		// CP949 (한글 2바이트, 로마 1바이트)
	char sendData2[] = u8"가";		// UTF-8 (한글 3바이트, 로마 1바이트)
	WCHAR sendData3[] = L"가";		// UTF-16 (한글 2바이트, 로마 2바이트)
	TCHAR sendData4[] = _T("가");	// 알아서 골라줘
	
	while (true)
	{
		Protocol::S_TEST pkt;
		pkt.set_id(1000);
		pkt.set_hp(200);
		pkt.set_attack(20);
		{
			Protocol::BuffData* buf = pkt.add_buffs();
			buf->set_buffid(100);
			buf->set_remaintime(3.5f);
			buf->add_victims(5);
		}
		{
			Protocol::BuffData* buf = pkt.add_buffs();
			buf->set_buffid(200);
			buf->set_remaintime(2.3f);
			buf->add_victims(4);
			buf->add_victims(1);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		
		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}