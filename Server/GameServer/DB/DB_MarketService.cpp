#include "pch.h"
#include "DB_MarketService.h"
#include "DB_AccountService.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"
#include "DBManager.h"
#include "ObjectManager.h"

bool DB_MarketService::PurchaseMarketItem(int32 buyerId, int32 itemId, shared_ptr<Room> room,  int32 slot,int32& retSellerId) {
	DBConnection* dbConn = GDBConnectionPool->Pop();
	ASSERT_CRASH(dbConn != nullptr);

	LOG_EVENT(Color::WHITE, L"[DB] Starting transaction for purchasing market item...\n");
	
	if (!dbConn->BeginTransaction()) {
		LOG_EVENT(Color::RED, L"[DB] Failed to begin transaction.\n");
		GDBConnectionPool->Push(dbConn);
		return false;
	}

	try {
		// MarketItem ���̺��� sellerId�� price ��ȸ
		int32 sellerId = 0;
		int32 price = 0;

		DBBind<1, 2> queryBind(*dbConn, L"SELECT SellerId, Price FROM MarketItem WHERE ItemId = ?");
		queryBind.BindParam(0, itemId);
		queryBind.BindCol(0, OUT sellerId);
		queryBind.BindCol(1, OUT price);

		if (!queryBind.Execute() || !dbConn->Fetch()) {
			throw std::runtime_error("Failed to retrieve sellerId and price from MarketItem.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Fetched sellerId=%d, price=%d for itemId=%d.\n", sellerId, price, itemId);

		retSellerId = sellerId;

		// ������ ��� ����
		if (!DB_AccountService::ChangeGold(dbConn, buyerId, -price, room)) {
			throw std::runtime_error("Failed to deduct gold from buyer.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Deducted %d gold from buyerId=%d.\n", price, buyerId);


		// �Ǹ��� ��� ����
		if (!DB_AccountService::ChangeGold(dbConn, sellerId, price, room)) {
			throw std::runtime_error("Failed to add gold to seller.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Added %d gold to sellerId=%d.\n", price, sellerId);


		DBBind<3, 0> updateItemBind(*dbConn, L"UPDATE ItemDB SET UserId = ?, IsOnMarket = 0, Slot = ? WHERE ItemId = ?");
		updateItemBind.BindParam(0, buyerId);
		updateItemBind.BindParam(1, slot);
		updateItemBind.BindParam(2, itemId);
	
		

		if (!updateItemBind.Execute()) {
			throw std::runtime_error("Failed to update item ownership.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Updated ownership of itemId=%d to buyerId=%d.\n", itemId, buyerId);


		// MarketItem ���̺��� ������ ����
		DBBind<1, 0> removeMarketItemBind(*dbConn, L"DELETE FROM MarketItem WHERE ItemId = ?");
		removeMarketItemBind.BindParam(0, itemId);

		if (!removeMarketItemBind.Execute()) {
			throw std::runtime_error("Failed to remove item from MarketItem.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Removed itemId=%d from MarketItem table.\n", itemId);

		if (!dbConn->CommitTransaction()) {
			throw std::runtime_error("Failed to commit transaction.");
		}
		LOG_EVENT(Color::GREEN, L"[DB] Transaction committed successfully.\n");


		// ĳ�� ������ ������Ʈ
		auto item = GDBManager->GetItem(itemId);
		if (item) {
			item->SetUserStatus(GDBManager->GetUserStatus(buyerId));
			item->SetOnMarket(false);
		}

		auto marketItem = GDBManager->GetMarketItem(itemId);
		if (marketItem) {
			GDBManager->marketItems.erase(itemId);
		}
		LOG_EVENT(Color::WHITE, L"[DB] Updated cache for itemId=%d.\n", itemId);

		GDBConnectionPool->Push(dbConn);
		return true;

	}
	catch (const std::exception& e) {
		// Ʈ����� �ѹ�
		dbConn->RollbackTransaction();
		GDBConnectionPool->Push(dbConn);
		std::wcerr << L"PurchaseMarketItem Error: " << e.what() << std::endl;
		return false;
	}
}


bool DB_MarketService::RegisterMarketItem(int32 sellerId, int32 itemId, int32 price, shared_ptr<ItemDB>& registerItem) {
	DBConnection* dbConn = GDBConnectionPool->Pop();
	ASSERT_CRASH(dbConn != nullptr);

	LOG_EVENT(Color::WHITE, L"[DB] Starting transaction for registering market item...\n");
	if (!dbConn->BeginTransaction()) {
		GDBConnectionPool->Push(dbConn);
		return false;
	}

	try {
		// MarketItem ���̺� ���
		{
			DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO MarketItem (ItemId, SellerId, Price) VALUES (?, ?, ?)");
			dbBind.BindParam(0, itemId);
			dbBind.BindParam(1, sellerId);
			dbBind.BindParam(2, price);

			ASSERT_CRASH(dbBind.Execute());
			LOG_EVENT(Color::GREEN, L"[DB] Registered itemId=%d for sellerId=%d at price=%d.\n", itemId, sellerId, price);
		}

		// ItemDB ���̺��� IsOnMarket ������Ʈ
		{
			DBBind<1, 0> dbBind(*dbConn, L"UPDATE ItemDB SET IsOnMarket = 1 WHERE ItemId = ?");
			dbBind.BindParam(0, itemId);

			ASSERT_CRASH(dbBind.Execute());
			LOG_EVENT(Color::GREEN, L"[DB] Updated IsOnMarket for itemId=%d.\n", itemId);

		}

		// ĳ�̵� ������ ������Ʈ
		auto item = GDBManager->GetItem(itemId);
		item->SetOnMarket(true);
		item->SetEquipped(false);


		// MarketItem�� ĳ�� �߰�
		auto newMarketItem = std::make_shared<MarketItem>();
		newMarketItem->SetItem(item);
		newMarketItem->SetItemId(itemId);
		newMarketItem->SetSellerId(sellerId);
		newMarketItem->SetPrice(price);
		GDBManager->AddMarketItem(newMarketItem);

		dbConn->CommitTransaction(); // Ʈ����� Ŀ��
		GDBConnectionPool->Push(dbConn);

		registerItem = item;
		LOG_EVENT(Color::WHITE, L"[DB] Updated cache for market itemId=%d.\n", itemId);

		return true;
	}
	catch (const std::exception& e) {
		// Ʈ����� �ѹ�
		dbConn->RollbackTransaction();
		GDBConnectionPool->Push(dbConn);
		std::wcerr << L"PurchaseMarketItem Error: " << e.what() << std::endl;
		return false;
	}
}

