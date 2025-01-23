#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	return true;
}

bool Handle_S_ENTER_ROOM(PacketSessionRef& session, Protocol::S_ENTER_ROOM& pkt)
{
	return false;
}

bool Handle_S_LEAVE_ROOM(PacketSessionRef& session, Protocol::S_LEAVE_ROOM& pkt)
{
	return false;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	return false;
}

bool Handle_S_ATTACK(PacketSessionRef& session, Protocol::S_ATTACK& pkt)
{
	return false;
}

bool Handle_S_UPDATE_RESOURCE(PacketSessionRef& session, Protocol::S_UPDATE_RESOURCE& pkt)
{
	return false;
}

bool Handle_S_DIE(PacketSessionRef& session, Protocol::S_DIE& pkt)
{
	return false;
}

bool Handle_S_PING(PacketSessionRef& session, Protocol::S_PING& pkt)
{
	return false;
}

bool Handle_S_USER_STATUS(PacketSessionRef& session, Protocol::S_USER_STATUS& pkt)
{
	return false;
}

bool Handle_S_ITEM_LIST(PacketSessionRef& session, Protocol::S_ITEM_LIST& pkt)
{
	return false;
}

bool Handle_S_ADD_ITEM(PacketSessionRef& session, Protocol::S_ADD_ITEM& pkt)
{
	return false;
}

bool Handle_S_CHANGE_CURRENCY(PacketSessionRef& session, Protocol::S_CHANGE_CURRENCY& pkt)
{
	return false;
}

bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt)
{
	return false;
}

bool Handle_S_CHANGE_ITEM_INFO(PacketSessionRef& session, Protocol::S_CHANGE_ITEM_INFO& pkt)
{
	return false;
}

bool Handle_S_REMOVE_ITEM(PacketSessionRef& session, Protocol::S_REMOVE_ITEM& pkt)
{
	return false;
}

bool Handle_S_MARKET_ITEM_LIST(PacketSessionRef& session, Protocol::S_MARKET_ITEM_LIST& pkt)
{
	return false;
}

bool Handle_S_PURCHASE_MARKET_ITEM(PacketSessionRef& session, Protocol::S_PURCHASE_MARKET_ITEM& pkt)
{
	return false;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	return true;
}
