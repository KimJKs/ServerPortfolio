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
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_SIGN_UP(PacketSessionRef& session, Protocol::C_SIGN_UP& pkt);
bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_ENTER_ROOM(PacketSessionRef& session, Protocol::C_ENTER_ROOM& pkt);
bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_POS_SYNC(PacketSessionRef& session, Protocol::C_POS_SYNC& pkt);
bool Handle_C_ATTACK(PacketSessionRef& session, Protocol::C_ATTACK& pkt);
bool Handle_C_PONG(PacketSessionRef& session, Protocol::C_PONG& pkt);
bool Handle_C_SKILL(PacketSessionRef& session, Protocol::C_SKILL& pkt);
bool Handle_C_USE_ITEM(PacketSessionRef& session, Protocol::C_USE_ITEM& pkt);
bool Handle_C_DROP_ITEM(PacketSessionRef& session, Protocol::C_DROP_ITEM& pkt);
bool Handle_C_REQUEST_MARKET_ITEM_LIST(PacketSessionRef& session, Protocol::C_REQUEST_MARKET_ITEM_LIST& pkt);
bool Handle_C_PURCHASE_MARKET_ITEM(PacketSessionRef& session, Protocol::C_PURCHASE_MARKET_ITEM& pkt);
bool Handle_C_REGISTER_MARKET_ITEM(PacketSessionRef& session, Protocol::C_REGISTER_MARKET_ITEM& pkt);
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt);
bool Handle_C_WHISPER(PacketSessionRef& session, Protocol::C_WHISPER& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_SIGN_UP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SIGN_UP>(Handle_C_SIGN_UP, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ENTER_ROOM>(Handle_C_ENTER_ROOM, session, buffer, len); };
		GPacketHandler[PKT_C_LEAVE_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_LEAVE_GAME>(Handle_C_LEAVE_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_POS_SYNC] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_POS_SYNC>(Handle_C_POS_SYNC, session, buffer, len); };
		GPacketHandler[PKT_C_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ATTACK>(Handle_C_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_C_PONG] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PONG>(Handle_C_PONG, session, buffer, len); };
		GPacketHandler[PKT_C_SKILL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SKILL>(Handle_C_SKILL, session, buffer, len); };
		GPacketHandler[PKT_C_USE_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_USE_ITEM>(Handle_C_USE_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_DROP_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_DROP_ITEM>(Handle_C_DROP_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_REQUEST_MARKET_ITEM_LIST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_REQUEST_MARKET_ITEM_LIST>(Handle_C_REQUEST_MARKET_ITEM_LIST, session, buffer, len); };
		GPacketHandler[PKT_C_PURCHASE_MARKET_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PURCHASE_MARKET_ITEM>(Handle_C_PURCHASE_MARKET_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_REGISTER_MARKET_ITEM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_REGISTER_MARKET_ITEM>(Handle_C_REGISTER_MARKET_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, session, buffer, len); };
		GPacketHandler[PKT_C_WHISPER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_WHISPER>(Handle_C_WHISPER, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEAVE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_S_LEAVE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_DESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_S_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::S_UPDATE_RESOURCE& pkt) { return MakeSendBuffer(pkt, PKT_S_UPDATE_RESOURCE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DIE& pkt) { return MakeSendBuffer(pkt, PKT_S_DIE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PING& pkt) { return MakeSendBuffer(pkt, PKT_S_PING); }
	static SendBufferRef MakeSendBuffer(Protocol::S_USER_STATUS& pkt) { return MakeSendBuffer(pkt, PKT_S_USER_STATUS); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ITEM_LIST& pkt) { return MakeSendBuffer(pkt, PKT_S_ITEM_LIST); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ADD_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_ADD_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHANGE_CURRENCY& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_CURRENCY); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_S_SKILL); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHANGE_ITEM_INFO& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGE_ITEM_INFO); }
	static SendBufferRef MakeSendBuffer(Protocol::S_REMOVE_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_REMOVE_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MARKET_ITEM_LIST& pkt) { return MakeSendBuffer(pkt, PKT_S_MARKET_ITEM_LIST); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PURCHASE_MARKET_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_PURCHASE_MARKET_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHAT); }

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