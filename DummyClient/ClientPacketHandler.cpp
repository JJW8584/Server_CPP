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
	struct BuffsListItem
	{
		uint32 buffId;
		float remainTime;

		// Victims List
		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{

		}
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
		if (packetSize < size)
			return false;

		// 버프 오프셋(시작위치)부터 버프의 개수만큼 셌는데 패킷 크기를 넘어가면 문제 있음
		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		size += buffsCount * sizeof(BuffsListItem);

		BuffsList buffsList = GetBuffsList();
		for (int32 i = 0; i < buffsList.Count(); i++)
		{

		}

		// 계산한 크기가 저장된 패킷 크기랑 다르면 문제가 있음
		if (size != packetSize)
			return false;
	
		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffsList()
	{
		// 바이트로 선언하여 덧셈 원활하게 함
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}
};
#pragma pack()

// [ PKT_S_TEST ][ BuffListItem  BuffListItem  BuffListItem ]
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	if (len < sizeof(PKT_S_TEST))
		return;

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);
	//PKT_S_TEST pkt;
	//br >> pkt;

	if (pkt->Validate() == false)
		return;

	/*cout << "ID: " << pkt.id
		<< " HP: " << pkt.hp
		<< " Attack: " << pkt.attack << endl;*/

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "BuffCount : " << buffs.Count() << endl;
	for (int32 i = 0; i < pkt->buffsCount; i++)
	{
		cout << "BuffInfo : " << buffs[i].buffId << ' ' << buffs[i].remainTime << endl;
	}
}