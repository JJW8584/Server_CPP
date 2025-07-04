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
		vector<BuffData> buffs{ BuffData{100, 1.3f}, BuffData{130, 1.8f}, BuffData{200, 2.3f} };
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_TEST(1001, 150, 10, buffs);
		
		GSessionManager.Broadcast(sendBuffer);

		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}