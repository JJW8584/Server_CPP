#pragma once

enum
{
	S_TEST = 1
};

// 패킷 설계 temp
struct BuffData
{
	int buffId;
	float remainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs);
	
};

