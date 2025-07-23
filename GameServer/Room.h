#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
public:
	// 싱글 스레드처럼 코딩
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// 멀티스레드 환경에서는 일감으로 접근
	virtual void FlushJob() override;

private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom;