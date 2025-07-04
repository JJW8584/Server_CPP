#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}

	
}

#pragma pack(1)
// [ PKT_S_TEST ][ BuffListItem  BuffListItem  BuffListItem ]
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint32 buffId;
		float remainTime;
	};
	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더
	uint64 id; // 8
	uint32 hp; // 4
	uint16 attack; // 2

	uint16 buffsOffset;
	uint16 buffsCount;

	bool Validate()
	{
		uint32 size = 0;

		size += sizeof(PKT_S_TEST);
		size += buffsCount * sizeof(BuffListItem);
		// 계산한 크기가 저장된 패킷 크기랑 다르면 문제가 있음
		if (size != packetSize)
			return false;

		// 버프 오프셋(시작위치)부터 버프의 개수만큼 셌는데 패킷 크기를 넘어가면 문제 있음
		if (buffsOffset + buffsCount * sizeof(BuffListItem) > packetSize)
			return false;

		return true;
	}
	// 가변데이터
	//vector<BuffData> buffs;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	if (len < sizeof(PKT_S_TEST))
		return;

	PKT_S_TEST pkt;
	br >> pkt;

	if (pkt.Validate() == false)
		return;

	cout << "ID: " << pkt.id
		<< " HP: " << pkt.hp
		<< " Attack: " << pkt.attack << endl;

	vector<PKT_S_TEST::BuffListItem> buffs;

	buffs.resize(pkt.buffsCount);
	for (int i = 0; i < pkt.buffsCount; i++)
	{
		br >> buffs[i]; // pragma pack(1)했으므로 문제 없음
	}

	cout << "BuffCount : " << pkt.buffsCount << endl;
	for (int32 i = 0; i < pkt.buffsCount; i++)
	{
		cout << "BuffInfo : " << buffs[i].buffId << ' ' << buffs[i].remainTime << endl;
	}
}
