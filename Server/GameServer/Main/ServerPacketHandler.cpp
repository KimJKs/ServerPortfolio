#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"
#include <locale>
#include <codecvt>
#include "ObjectManager.h"
#include "DBManager.h"
#include "DB_AccountService.h"
#include "DB_MarketService.h"
#include "DB_ItemService.h"
#include "RoomManager.h"
#include <thread>
#include <future>

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_LOGIN Packet received: ID=%s\n", pkt.id().c_str());

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	Protocol::S_LOGIN loginPkt;

	// Ŭ���̾�Ʈ�κ��� ���� ���̵�� ��й�ȣ
	std::wstring username = Utils::StringToWString(pkt.id());
	std::wstring password = Utils::StringToWString(pkt.password());

	int32 userId = 0;
	int32 goldAmount = 0;

	// DB���� ���̵�� ��й�ȣ Ȯ��
	bool userExists = DB_AccountService::FetchUserInfo(username, password, userId, goldAmount);

	if (userExists) {
		// ������ �����ϸ� ���� ó��
		loginPkt.set_success(true);

		// �÷��̾� ����
		shared_ptr<Player> player = ObjectManager::Instance().Add<Player>();
		player->SetSession(gameSession);

		player->SetObjectName(Utils::WStringToString(username));

		gameSession->player = player;

		// �α��� �����ϰ� UserStatus ����
		auto userStatus = DB_AccountService::LoginUser(userId, false);
		gameSession->SetUserStatus(userStatus);

		// �÷��̾� ���� ����
		Protocol::PlayerInfo* playerInfo = loginPkt.mutable_playerinfo();
		playerInfo->set_name(pkt.id());
		playerInfo->set_goldamount(goldAmount);
	}
	else {
		// ������ �������� �ʰų� ��й�ȣ�� Ʋ�� ���
		loginPkt.set_success(false); 
	}

	// Ŭ���̾�Ʈ�� ���� ����
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
	gameSession->Send(sendBuffer);

	return userExists;
}

bool Handle_C_SIGN_UP(PacketSessionRef& session, Protocol::C_SIGN_UP& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_SIGN_UP Packet received: ID=%s\n", pkt.id().c_str());

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	std::wstring username = Utils::StringToWString(pkt.id());
	std::wstring password = Utils::StringToWString(pkt.password());

	int32 userDbId = 0;

	bool success = DB_AccountService::RegisterUser(username, password, userDbId);

	Protocol::S_LOGIN loginPkt;

	loginPkt.set_success(success);

	if (success)
	{
		// ������ �����ϸ� ���� ó��
		loginPkt.set_success(true);

		// �÷��̾� ����
		shared_ptr<Player> player = ObjectManager::Instance().Add<Player>();
		player->SetSession(gameSession);

		player->SetObjectName(Utils::WStringToString(username));

		gameSession->player = player;

		// �α��� �����ϰ� UserStatus ����
		auto userStatus = DB_AccountService::LoginUser(userDbId, false);
		gameSession->SetUserStatus(userStatus);

		// �÷��̾� ���� ����
		Protocol::PlayerInfo* playerInfo = loginPkt.mutable_playerinfo();
		playerInfo->set_name(pkt.id());
		playerInfo->set_goldamount(0);
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
	gameSession->Send(sendBuffer);
	//SEND_PACKET(loginPkt);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_ENTER_GAME Packet received\n");

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	std::shared_ptr<Player> player = gameSession->player;

	if (player)
		player->GetInventory().LoadPlayerItems(gameSession->GetUserStatus()->GetUserId(), player);

	if (pkt.isdummy() && !player)
	{
		// �÷��̾� ����
		player = ObjectManager::Instance().Add<Player>();
		player->SetSession(gameSession);
		player->SetObjectType(Protocol::ObjectType::PLAYER);

		player->SetPositionInfo(pkt.objectinfo().posinfo());
		player->SetObjectName(pkt.objectinfo().name());

		player->isDummy = true;

		gameSession->player = player;

	}

	// �⺻ �ʵ� ����
	shared_ptr<Room> room = GRoomManager->FindByMapId(0);

	room->DoAsync(&Room::Enter, std::static_pointer_cast<Object>(player));
	player->GetSession().lock()->_room = room;

	return true;
}

bool Handle_C_ENTER_ROOM(PacketSessionRef& session, Protocol::C_ENTER_ROOM& pkt)
{

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);

	std::shared_ptr<Player> player = gameSession->player;

	shared_ptr<Room> oldRoom = gameSession->_room.lock();

	int32 mapId = pkt.mapid();

	if (oldRoom != nullptr && oldRoom->GetMapId() == mapId)
	{
		return false;
	}

	if (oldRoom != nullptr)
	{
		oldRoom->DoAsync(&Room::Leave, player->GetId());
	}

	// �濡 ����
	shared_ptr<Room> newRoom = GRoomManager->FindByMapId(mapId);


	if (newRoom == nullptr)
	{
		// �ݹ� �۾�: �� ���� �� ó��
		GRoomManager->DoAsync(
			[&player, mapId]()
			{
				shared_ptr<Room> createdRoom = GRoomManager->Add(mapId);
				// �� ���� ó��
				if (player && createdRoom != nullptr)
				{
					createdRoom->DoAsync(&Room::Enter, std::static_pointer_cast<Object>(player));
					player->GetSession().lock()->_room = createdRoom;
				}
			}
		);

		return true;
	}

	newRoom->DoAsync(&Room::Enter, std::static_pointer_cast<Object>(gameSession->player));
	gameSession->_room = newRoom;

	return true;
}


bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	//LOG_EVENT(Color::WHITE, L"[PacketHandler] C_MOVE Packet received\n");

	auto gameSession = static_pointer_cast<GameSession>(session);

	if (gameSession->_room.lock() == nullptr)
		return false;

	gameSession->_room.lock()->DoAsync(&Room::HandleMove, gameSession->player, pkt.posinfo(), pkt.timestamp());

	return true;
}

bool Handle_C_POS_SYNC(PacketSessionRef& session, Protocol::C_POS_SYNC& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;
	if (!gameSession->_room.lock())
		return false;

	// �濡�� ��ġ ����ȭ ó��
	gameSession->_room.lock()->DoAsync(&Room::SyncPos, player, pkt.posinfo());

	return true;
}

bool Handle_C_ATTACK(PacketSessionRef& session, Protocol::C_ATTACK& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_ATTACK Packet received\n");

	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;
	if (!gameSession->_room.lock())
		return false;

	// �濡�� ���� ó��
	gameSession->_room.lock()->DoAsync(&Room::HandleAttack, player, pkt.dirx(), pkt.dirz());

	return true;
}

bool Handle_C_PONG(PacketSessionRef& session, Protocol::C_PONG& pkt)
{
	uint64_t clientTimestamp = pkt.timestamp();

	// ���� ����
	Protocol::S_PING pingPacket;
	pingPacket.set_timestamp(clientTimestamp);

	// Ŭ���̾�Ʈ�� ����
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pingPacket);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_USE_ITEM(PacketSessionRef& session, Protocol::C_USE_ITEM& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;
	if (!gameSession->_room.lock())
		return false;

	// �濡�� ������ ��� ó��
	gameSession->_room.lock()->DoAsync(&Room::HandleUseItem, player, pkt.itemdbid());

	return true;
}

bool Handle_C_DROP_ITEM(PacketSessionRef& session, Protocol::C_DROP_ITEM& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;
	if (!gameSession->_room.lock())
		return false;

	// �濡�� ������ ��� ó��
	gameSession->_room.lock()->DoAsync(&Room::HandleRemoveItem, player, pkt.itemdbid());

	return true;
}

bool Handle_C_SKILL(PacketSessionRef& session, Protocol::C_SKILL& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;

	auto room = gameSession->_room.lock();

	room->DoAsync(&Room::HandleSkill, player, pkt.skilltemplateid(), pkt.posinfo());

}

bool Handle_C_PURCHASE_MARKET_ITEM(PacketSessionRef& session, Protocol::C_PURCHASE_MARKET_ITEM& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;

	int32 sellerId = -1;

	if (!player->GetInventory().GetEmptySlot())
		return false;


	int32 slot = player->GetInventory().GetEmptySlot().value();


	// DB �۾��� ���� ���� ������ ���� ó��
	if (!DB_MarketService::PurchaseMarketItem(gameSession->GetUserStatus()->GetUserId(), pkt.itemddbid(), player->GetRoom(), slot, sellerId))
		return false;

	//shared_ptr<Item> item = Item::CreateItem(*GDBManager->GetItem(pkt.itemddbid()));
	shared_ptr<Item> item = nullptr;

	shared_ptr<Player> sellerPlayer = GRoomManager->FindPlayerByUserDbId(sellerId);


	if (sellerPlayer)
	{
		item = sellerPlayer->GetInventory().Get(pkt.itemddbid());
		sellerPlayer->GetInventory().Remove(pkt.itemddbid());
		Protocol::S_REMOVE_ITEM packet;
		{
			packet.set_removeditemdbid(pkt.itemddbid());
		}
		SendBufferRef sendBufferFirst = ServerPacketHandler::MakeSendBuffer(packet);
		sellerPlayer->GetSession().lock()->Send(sendBufferFirst);
	}
	else item = Item::CreateItem(*GDBManager->GetItem(pkt.itemddbid()));

	item->SetOnMarket(false);
	player->GetInventory().Add(item);

	Protocol::S_PURCHASE_MARKET_ITEM packet2;
	{

		Protocol::ItemInfo itemInfo = item->GetItemInfo();

		Protocol::ItemInfo* packetItemInfo = packet2.mutable_items();
		*packetItemInfo = itemInfo;
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet2);
	gameSession->Send(sendBuffer);
	//SEND_PACKET(packet2);


	return true;
}

bool Handle_C_REGISTER_MARKET_ITEM(PacketSessionRef& session, Protocol::C_REGISTER_MARKET_ITEM& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;

	shared_ptr<ItemDB> registerItem;

	// DB �۾��� ���� ���忡 ������ ���
	if (!DB_MarketService::RegisterMarketItem(gameSession->GetUserStatus()->GetUserId(), pkt.itemddbid(), pkt.cost(), OUT registerItem))
		return false;

	shared_ptr<Item> item = player->GetInventory().Get(pkt.itemddbid());

	item->SetOnMarket(true);
	if (item->IsEquipped())
	{
		item->SetEquipped(false);
		player->GetInventory().UnequipItem(item->GetItemType());
	}

	GDBManager->AddItem(registerItem);
	DB_ItemService::UpdateItem(registerItem->GetItemId(), item->GetCount(), item->IsEquipped());

	Protocol::S_CHANGE_ITEM_INFO packet;
	{
		Protocol::ItemInfo itemInfo = item->GetItemInfo();

		Protocol::ItemInfo* packetItemInfo = packet.add_items();
		*packetItemInfo = itemInfo;
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	gameSession->Send(sendBuffer);
	//SEND_PACKET(packet);


	return true;
}

bool Handle_C_REQUEST_MARKET_ITEM_LIST(PacketSessionRef& session, Protocol::C_REQUEST_MARKET_ITEM_LIST& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;

	Protocol::S_MARKET_ITEM_LIST marketListPacket;

	for (auto& [id, marketItem] : GDBManager->marketItems) {
		if (gameSession->GetUserStatus()->GetUserId() != marketItem->GetSellerId()) {
			Protocol::MarketItemInfo* itemInfo = marketListPacket.add_items();
			itemInfo->set_cost(marketItem->GetPrice());
			itemInfo->set_itemdbid(marketItem->GetItemId());
			itemInfo->set_templateid(marketItem->GetItem()->GetTemplateId());
		}
	}


	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(marketListPacket);
	gameSession->Send(sendBuffer);
	//SEND_PACKET(marketListPacket);

	return true;
}


bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_CHAT Packet receive.\n");

	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;


	shared_ptr<Room> room = gameSession->_room.lock();

	room->DoAsync(
		[pkt, player, room]()
		{
			Protocol::S_CHAT chatPacket;
			string prefix = player->GetObjectInfo().name() + " : ";

			chatPacket.set_prefix(prefix);
			chatPacket.set_content(pkt.message());
			chatPacket.set_senderid(pkt.senderid());
			chatPacket.set_type(Protocol::CHAT_TYPE_GENERAL);
			room->Broadcast(chatPacket, player);
		}
	);
	return true;
}

bool Handle_C_WHISPER(PacketSessionRef& session, Protocol::C_WHISPER& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_WHISPER Packet received.\n");

	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto sender = gameSession->player;

	// ������ �̸� ��������
	std::wstring receiverName = Utils::StringToWString(pkt.receivername());

	shared_ptr<Player> receiver = GRoomManager->FindPlayerByUserName(receiverName);


	if (receiver && receiver != sender) {
		// �����ڿ��� ����
		Protocol::S_CHAT receiverPacket;
		receiverPacket.set_prefix(Utils::WStringToString(gameSession->GetUserStatus()->GetUsername() + L"�� ��ſ��� : "));
		receiverPacket.set_content(pkt.message());
		receiverPacket.set_type(Protocol::CHAT_TYPE_WHISPER);
		receiver->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(receiverPacket));

		// �۽��ڿ��� ����
		Protocol::S_CHAT senderPacket;
		senderPacket.set_prefix(Utils::WStringToString(L"����� " + gameSession->GetUserStatus()->GetUsername() + L"���� : "));
		senderPacket.set_content(pkt.message());
		senderPacket.set_type(Protocol::CHAT_TYPE_WHISPER);
		sender->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(senderPacket));
	}
	else {
		// ����� �߸��� ��� �۽��ڿ��� ���� �޽��� ����
		Protocol::S_CHAT errorPacket;
		errorPacket.set_content(Utils::WStringToString(L"�߸��� ����Դϴ�."));
		errorPacket.set_type(Protocol::CHAT_TYPE_SYSTEM);
		sender->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(errorPacket));
	}


	return false;
}
