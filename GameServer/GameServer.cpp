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
		PKT_S_TEST_WRITE pktWriter(1001, 100, 10);

		PKT_S_TEST_WRITE::BuffsList buffList = pktWriter.ReserveBuffsList(3);
		buffList[0] = { 100, 1.3f };
		buffList[1] = { 130, 1.8f };
		buffList[2] = { 200, 2.3f };

		PKT_S_TEST_WRITE::BuffsVictimsList vic0 = pktWriter.ReserveBuffsVictimsList(&buffList[0], 2);
		{
			vic0[0] = (uint64)1000;
			vic0[1] = (uint64)2000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic1 = pktWriter.ReserveBuffsVictimsList(&buffList[1], 1);
		{
			vic1[0] = (uint64)1000;
		}

		PKT_S_TEST_WRITE::BuffsVictimsList vic2 = pktWriter.ReserveBuffsVictimsList(&buffList[2], 3);
		{
			vic2[0] = (uint64)1000;
			vic2[1] = (uint64)2000;
			vic2[2] = (uint64)3000;
		}

		SendBufferRef sendBuffer = pktWriter.CloseAndReturn();
		
		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}