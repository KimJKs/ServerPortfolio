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
#include "DBWorker.h"
#include <thread>
#include <future>

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_LOGIN Packet received: ID=%s\n", pkt.id().c_str());

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	std::wstring username = Utils::StringToWString(pkt.id());
	std::wstring password = Utils::StringToWString(pkt.password());

	GDBWorker->AddJob([gameSession, username, password, pkt]()
		{
			int32 userId = 0;
			bool userExists = DB_AccountService::FetchUserInfo(username, password, userId);

			shared_ptr<UserStatus> userStatus = nullptr;
			if (userExists)
			{
				userStatus = DB_AccountService::LoginUser(userId, false);
			}

			GRoomManager->DoAsync([gameSession, userExists, userStatus, userId, pkt]()
				{
					Protocol::S_LOGIN loginPkt;

					if (userExists)
					{
						loginPkt.set_success(true);

						shared_ptr<Player> player = ObjectManager::Instance().Add<Player>();
						player->SetSession(gameSession);
						player->SetObjectName(pkt.id());
						gameSession->player = player;

						gameSession->SetUserStatus(userStatus);

						Protocol::PlayerInfo* playerInfo = loginPkt.mutable_playerinfo();
						playerInfo->set_name(pkt.id());
						playerInfo->set_goldamount(userStatus->GetGoldAmount());
					}
					else
					{
						loginPkt.set_success(false);
					}

					SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
					gameSession->Send(sendBuffer);
				},true);
		});

	return true;
}


bool Handle_C_SIGN_UP(PacketSessionRef& session, Protocol::C_SIGN_UP& pkt)
{
	LOG_EVENT(Color::WHITE, L"[PacketHandler] C_SIGN_UP Packet received: ID=%s\n", pkt.id().c_str());

	shared_ptr<GameSession> gameSession = static_pointer_cast<GameSession>(session);
	std::wstring username = Utils::StringToWString(pkt.id());
	std::wstring password = Utils::StringToWString(pkt.password());

	GDBWorker->AddJob([gameSession, username, password, pkt]()
		{
			int32 userDbId = 0;
			bool success = DB_AccountService::RegisterUser(username, password, userDbId);

			shared_ptr<UserStatus> userStatus = nullptr;
			if (success)
			{
				userStatus = DB_AccountService::LoginUser(userDbId, false);
			}

			GRoomManager->DoAsync([gameSession, success, userStatus, userDbId, pkt]()
				{
					Protocol::S_LOGIN loginPkt;
					loginPkt.set_success(success);

					if (success)
					{
						shared_ptr<Player> player = ObjectManager::Instance().Add<Player>();
						player->SetSession(gameSession);
						player->SetObjectName(pkt.id());
						gameSession->player = player;

						gameSession->SetUserStatus(userStatus);

						Protocol::PlayerInfo* playerInfo = loginPkt.mutable_playerinfo();
						playerInfo->set_name(pkt.id());
						playerInfo->set_goldamount(0);
					}

					SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
					gameSession->Send(sendBuffer);
				}, true);
		});

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
		// 플레이어 생성
		player = ObjectManager::Instance().Add<Player>();
		player->SetSession(gameSession);
		player->SetObjectType(Protocol::ObjectType::PLAYER);

		player->SetPositionInfo(pkt.objectinfo().posinfo());
		player->SetObjectName(pkt.objectinfo().name());

		player->isDummy = true;

		gameSession->player = player;

	}

	// 기본 필드 입장
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

	// 방에 입장
	shared_ptr<Room> newRoom = GRoomManager->FindByMapId(mapId);


	if (newRoom == nullptr)
	{
		// 콜백 작업: 방 생성 후 처리
		GRoomManager->DoAsync(
			[&player, mapId]()
			{
				shared_ptr<Room> createdRoom = GRoomManager->Add(mapId);
				// 방 입장 처리
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

	// 방에서 위치 동기화 처리
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

	// 방에서 공격 처리
	gameSession->_room.lock()->DoAsync(&Room::HandleAttack, player, pkt.dirx(), pkt.dirz());

	return true;
}

bool Handle_C_PONG(PacketSessionRef& session, Protocol::C_PONG& pkt)
{
	uint64_t clientTimestamp = pkt.timestamp();

	// 응답 생성
	Protocol::S_PING pingPacket;
	pingPacket.set_timestamp(clientTimestamp);

	// 클라이언트로 응답
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

	// 방에서 아이템 사용 처리
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

	// 방에서 아이템 드랍 처리
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
	int32 userId = gameSession->GetUserStatus()->GetUserId();
	int32 itemDbId = pkt.itemddbid();

	if (!player->GetInventory().GetEmptySlot())
		return false;

	int32 slot = player->GetInventory().GetEmptySlot().value();

	// DBWorker에서 시장 아이템 구매를 비동기 처리
	GDBWorker->AddJob([gameSession, player, userId, itemDbId, slot]()
		{
			int32 sellerId = -1;

			// DB에서 시장 아이템 구매 처리
			if (!DB_MarketService::PurchaseMarketItem(userId, itemDbId, player->GetRoom(), slot, sellerId))
				return;

			shared_ptr<Player> sellerPlayer = GRoomManager->FindPlayerByUserDbId(sellerId);

			// 워크 스레드에서 실행
			GRoomManager->DoAsync([gameSession, player, sellerPlayer, itemDbId]()
				{
					shared_ptr<Item> item = nullptr;

					if (!gameSession || !player)
						return;

					if (sellerPlayer)
					{
						
						item = sellerPlayer->GetInventory().Get(itemDbId);
						if (!item)
							return;

						sellerPlayer->GetInventory().Remove(itemDbId);

						Protocol::S_REMOVE_ITEM packet;
						packet.set_removeditemdbid(itemDbId);

						SendBufferRef sendBufferFirst = ServerPacketHandler::MakeSendBuffer(packet);
						sellerPlayer->GetSession().lock()->Send(sendBufferFirst);
					}
					else
					{
						item = Item::CreateItem(*GDBManager->GetItem(itemDbId));
					}

					item->SetOnMarket(false);
					player->GetInventory().Add(item);

					// 클라이언트에 변경된 아이템 정보 전송
					Protocol::S_PURCHASE_MARKET_ITEM packet2;
					{
						Protocol::ItemInfo itemInfo = item->GetItemInfo();
						Protocol::ItemInfo* packetItemInfo = packet2.mutable_items();
						*packetItemInfo = itemInfo;
					}

					SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet2);
					gameSession->Send(sendBuffer);
				}, true);
		});

	return true;
}

bool Handle_C_REGISTER_MARKET_ITEM(PacketSessionRef& session, Protocol::C_REGISTER_MARKET_ITEM& pkt)
{
	auto gameSession = std::static_pointer_cast<GameSession>(session);
	if (!gameSession || !gameSession->player)
		return false;

	auto player = gameSession->player;
	int32 userId = gameSession->GetUserStatus()->GetUserId();
	int32 itemDbId = pkt.itemddbid();
	int32 cost = pkt.cost();

	// DBWorker에서 시장 아이템 등록을 비동기 처리
	GDBWorker->AddJob([gameSession, player, userId, itemDbId, cost]()
		{
			shared_ptr<ItemDB> registerItem;
			if (!DB_MarketService::RegisterMarketItem(userId, itemDbId, cost, OUT registerItem))
				return;

			// 워크 스레드에서 아이템 상태 업데이트 및 클라이언트 응답 전송
			GRoomManager->DoAsync([gameSession, player, registerItem, itemDbId]()
				{
					if (!gameSession || !player)
						return;

					auto item = player->GetInventory().Get(itemDbId);
					if (!item)
						return;

					item->SetOnMarket(true);
					if (item->IsEquipped())
					{
						item->SetEquipped(false);
						player->GetInventory().UnequipItem(item->GetItemType());
					}

					GDBManager->AddItem(registerItem);

					// 아이템 상태 업데이트를 비동기 처리
					GDBWorker->AddJob([itemDbId, count = item->GetCount(), equipped = item->IsEquipped()]()
						{
							DB_ItemService::UpdateItem(itemDbId, count, equipped);
						});

					// 클라이언트에 변경된 아이템 정보 전송
					Protocol::S_CHANGE_ITEM_INFO packet;
					auto* packetItemInfo = packet.add_items();
					*packetItemInfo = item->GetItemInfo();

					SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
					gameSession->Send(sendBuffer);
				});
		});

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

	// 수신자 이름 가져오기
	std::wstring receiverName = Utils::StringToWString(pkt.receivername());

	shared_ptr<Player> receiver = GRoomManager->FindPlayerByUserName(receiverName);


	if (receiver && receiver != sender) {
		// 수신자에게 전송
		Protocol::S_CHAT receiverPacket;
		receiverPacket.set_prefix(Utils::WStringToString(gameSession->GetUserStatus()->GetUsername() + L"이 당신에게 : "));
		receiverPacket.set_content(pkt.message());
		receiverPacket.set_type(Protocol::CHAT_TYPE_WHISPER);
		receiver->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(receiverPacket));

		// 송신자에게 전송
		Protocol::S_CHAT senderPacket;
		senderPacket.set_prefix(Utils::WStringToString(L"당신이 " + gameSession->GetUserStatus()->GetUsername() + L"에게 : "));
		senderPacket.set_content(pkt.message());
		senderPacket.set_type(Protocol::CHAT_TYPE_WHISPER);
		sender->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(senderPacket));
	}
	else {
		// 대상이 잘못된 경우 송신자에게 오류 메시지 전송
		Protocol::S_CHAT errorPacket;
		errorPacket.set_content(Utils::WStringToString(L"잘못된 대상입니다."));
		errorPacket.set_type(Protocol::CHAT_TYPE_SYSTEM);
		sender->GetSession().lock()->Send(ServerPacketHandler::MakeSendBuffer(errorPacket));
	}


	return false;
}
