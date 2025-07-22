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

void HealByValue(int64 target, int32 value)
{
	cout << target << "한테 힐 " << value << endl;
}

class Knight
{
public:
	void HealMe(int32 value)
	{
		cout << "Heal Me!" << endl;
	}
};

int main()
{
	// Test Job
	{
		FuncJob<void, int64, int32> job(HealByValue, 100, 10);
		job.Excute();
	}
	{
		Knight k1;
		MemberJob job2(&k1, &Knight::HealMe, 10);
		job2.Excute();
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