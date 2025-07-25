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
#include "Player.h"
#include <functional>

class Knight : public enable_shared_from_this<Knight>
{
public:
	void HealMe(int32 value)
	{
		_hp += value;
		cout << "Heal Me" << endl;
	}

	void Test()
	{
		auto job = [self = shared_from_this()]()
			{
				self->HealMe(self->_hp);
			};
	}

private:
	int32 _hp = 100;
};

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

	GThreadManager->Join();
}