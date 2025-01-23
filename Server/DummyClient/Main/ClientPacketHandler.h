#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "S1.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// Utils Func
class US1NetworkManager* GetWorldNetwork(const PacketSessionRef& Session);

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_C_SIGN_UP = 1001,
	PKT_S_LOGIN = 1002,
	PKT_C_ENTER_GAME = 1003,
	PKT_C_ENTER_ROOM = 1004,
	PKT_S_ENTER_ROOM = 1005,
	PKT_C_LEAVE_GAME = 1006,
	PKT_S_LEAVE_ROOM = 1007,
	PKT_S_SPAWN = 1008,
	PKT_S_DESPAWN = 1009,
	PKT_C_MOVE = 1010,
	PKT_S_MOVE = 1011,
	PKT_C_POS_SYNC = 1012,
	PKT_C_ATTACK = 1013,
	PKT_S_ATTACK = 1014,
	PKT_S_UPDATE_RESOURCE = 1015,
	PKT_S_DIE = 1016,
	PKT_S_PING = 1017,
	PKT_C_PONG = 1018,
	PKT_S_USER_STATUS = 1019,
	PKT_S_ITEM_LIST = 1020,
	PKT_S_ADD_ITEM = 1021,
	PKT_S_CHANGE_CURRENCY = 1022,
	PKT_C_SKILL = 1023,
	PKT_S_SKILL = 1024,
	PKT_C_USE_ITEM = 1025,
	PKT_S_CHANGE_ITEM_INFO = 1026,
	PKT_C_DROP_ITEM = 1027,
	PKT_S_REMOVE_ITEM = 1028,
	PKT_C_REQUEST_MARKET_ITEM_LIST = 1029,
	PKT_S_MARKET_ITEM_LIST = 1030,
	PKT_C_PURCHASE_MARKET_ITEM = 1031,
	PKT_C_REGISTER_MARKET_ITEM = 1032,
	PKT_S_PURCHASE_MARKET_ITEM = 1033,
	PKT_C_CHAT = 1034,
	PKT_S_CHAT = 1035,
	PKT_C_WHISPER = 1036,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_ROOM(PacketSessionRef& session, Protocol::S_ENTER_ROOM& pkt);
bool Handle_S_LEAVE_ROOM(PacketSessionRef& session, Protocol::S_LEAVE_ROOM& pkt);
bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt);
bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt);
bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt);
bool Handle_S_ATTACK(PacketSessionRef& session, Protocol::S_ATTACK& pkt);
bool Handle_S_UPDATE_RESOURCE(PacketSessionRef& session, Protocol::S_UPDATE_RESOURCE& pkt);
bool Handle_S_DIE(PacketSessionRef& session, Protocol::S_DIE& pkt);
bool Handle_S_PING(PacketSessionRef& session, Protocol::S_PING& pkt);
bool Handle_S_USER_STATUS(PacketSessionRef& session, Protocol::S_USER_STATUS& pkt);
bool Handle_S_ITEM_LIST(PacketSessionRef& session, Protocol::S_ITEM_LIST& pkt);
bool Handle_S_ADD_ITEM(PacketSessionRef& session, Protocol::S_ADD_ITEM& pkt);
bool Handle_S_CHANGE_CURRENCY(PacketSessionRef& session, Protocol::S_CHANGE_CURRENCY& pkt);
bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt);
bool Handle_S_CHANGE_ITEM_INFO(PacketSessionRef& session, Protocol::S_CHANGE_ITEM_INFO& pkt);
bool Handle_S_REMOVE_ITEM(PacketSessionRef& session, Protocol::S_REMOVE_ITEM& pkt);
bool Handle_S_MARKET_ITEM_LIST(PacketSessionRef& session, Protocol::S_MARKET_ITEM_LIST& pkt);
bool Handle_S_PURCHASE_MARKET_ITEM(PacketSessionRef& session, Protocol::S_PURCHASE_MARKET_ITEM& pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_ROOM>(Handle_S_ENTER_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_LEAVE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEAVE_ROOM>(Handle_S_LEAVE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_SPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SPAWN>(Handle_S_SPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_DESPAWN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DESPAWN>(Handle_S_DESPAWN, session, buffer, len); };
		GPacketHandler[PKT_S_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MOVE>(Handle_S_MOVE, session, buffer, len); };
		GPacketHandler[PKT_S_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ATTACK>(Handle_S_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_S_UPDATE_RESOURCE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_UPDATE_RESOURCE>(Handle_S_UPDATE_RESOURCE, session, buffer, len); };
		GPacketHandler[PKT_S_DIE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DIE>(Handle_S_DIE, session, buffer, len); };
		GPacketHandler[PKT_S_PING] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PING>(Handle_S_PING, session, buffer, len); };
		GPacketHandler[PKT_S_USER_STATUS] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_USER_STATUS>(Handle_S_USER_STATUS, session, buffer, len); };
		GPacketHandler[PKT_S_ITEM_LIST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ITEM_LIST>(Handle_S_ITEM_LIST, session, buffer, len); };
		GPacketHandler[PKT_S_ADD_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ADD_ITEM>(Handle_S_ADD_ITEM, session, buffer, len); };
		GPacketHandler[PKT_S_CHANGE_CURRENCY] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHANGE_CURRENCY>(Handle_S_CHANGE_CURRENCY, session, buffer, len); };
		GPacketHandler[PKT_S_SKILL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SKILL>(Handle_S_SKILL, session, buffer, len); };
		GPacketHandler[PKT_S_CHANGE_ITEM_INFO] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHANGE_ITEM_INFO>(Handle_S_CHANGE_ITEM_INFO, session, buffer, len); };
		GPacketHandler[PKT_S_REMOVE_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_REMOVE_ITEM>(Handle_S_REMOVE_ITEM, session, buffer, len); };
		GPacketHandler[PKT_S_MARKET_ITEM_LIST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MARKET_ITEM_LIST>(Handle_S_MARKET_ITEM_LIST, session, buffer, len); };
		GPacketHandler[PKT_S_PURCHASE_MARKET_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PURCHASE_MARKET_ITEM>(Handle_S_PURCHASE_MARKET_ITEM, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SIGN_UP& pkt) { return MakeSendBuffer(pkt, PKT_C_SIGN_UP); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LEAVE_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::C_POS_SYNC& pkt) { return MakeSendBuffer(pkt, PKT_C_POS_SYNC); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_C_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PONG& pkt) { return MakeSendBuffer(pkt, PKT_C_PONG); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_C_SKILL); }
	static SendBufferRef MakeSendBuffer(Protocol::C_USE_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_USE_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DROP_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_DROP_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_REQUEST_MARKET_ITEM_LIST& pkt) { return MakeSendBuffer(pkt, PKT_C_REQUEST_MARKET_ITEM_LIST); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PURCHASE_MARKET_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_PURCHASE_MARKET_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_REGISTER_MARKET_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_REGISTER_MARKET_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_WHISPER& pkt) { return MakeSendBuffer(pkt, PKT_C_WHISPER); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};