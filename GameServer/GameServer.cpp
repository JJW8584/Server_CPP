#include "pch.h"
#include "CorePch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include <chrono>
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

int main()
{
	ClientPacketHandler::Init();

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
	
	while (true)
	{
		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}