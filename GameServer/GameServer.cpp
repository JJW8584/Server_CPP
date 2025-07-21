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
#include "Job.h"
#include "Room.h"

int main()
{
	// Test Job
	{
		// [요청 내용] : 1번 플레이어에게 10만큼 힐
		// 행동 : heal
		// 인자 : 1번 유저, 힐량 10

		HealJob healJob;
		healJob._target = 1;
		healJob._healValue = 10;

		healJob.Excute();
	}


	// Job

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
		GRoom.FlushJob();
		std::this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}