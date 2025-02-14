#include "pch.h"
#include "DBManager.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "Room.h"
#include "DB_ItemService.h"

shared_ptr<DBManager> GDBManager = make_shared<DBManager>();


void DBManager::Init()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(8, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\ProjectModels;Database=ServerDb;Trusted_Connection=Yes;"));
    LOG_EVENT(Color::GREEN, L"[DB] Connected to database successfully.\n");


	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

        LOG_EVENT(Color::WHITE, L"[DB] Creating tables if not exist...\n");


        // UserAccount 테이블 생성
        ASSERT_CRASH(dbConn->Execute(L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='UserAccount' AND xtype='U') "
            L"CREATE TABLE UserAccount ("
            L"UserId INT IDENTITY(1,1) PRIMARY KEY, "
            L"Username NVARCHAR(50) UNIQUE NOT NULL, "
            L"Password NVARCHAR(50) NOT NULL)"));
        LOG_EVENT(Color::GREEN, L"[DB] Table 'UserAccount' created or already exists.\n");

        // UserStatus 테이블 생성
        ASSERT_CRASH(dbConn->Execute(L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='UserStatus' AND xtype='U') "
            L"CREATE TABLE UserStatus ("
            L"UserId INT PRIMARY KEY, "
            L"GoldAmount INT DEFAULT 0, "
            L"IsLoggedIn BIT DEFAULT 0, "
            L"Username NVARCHAR(50), "
            L"FOREIGN KEY (UserId) REFERENCES UserAccount(UserId) "
            L"ON DELETE CASCADE)"));
        LOG_EVENT(Color::GREEN, L"[DB] Table 'UserStatus' created or already exists.\n");

        // ItemDB 테이블 생성
        ASSERT_CRASH(dbConn->Execute(L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='ItemDB' AND xtype='U') "
            L"CREATE TABLE ItemDB ("
            L"ItemId INT IDENTITY(1,1) PRIMARY KEY, "
            L"TemplateId INT NOT NULL, "
            L"Count INT DEFAULT 1, "
            L"Slot INT DEFAULT 0, "
            L"Equipped BIT DEFAULT 0, "
            L"IsOnMarket BIT DEFAULT 0, "
            L"UserId INT NOT NULL, "
            L"FOREIGN KEY (UserId) REFERENCES UserStatus(UserId) "
            L"ON DELETE CASCADE)"));
        LOG_EVENT(Color::GREEN, L"[DB] Table 'ItemDB' created or already exists.\n");

        // MarketItem 테이블 생성
        ASSERT_CRASH(dbConn->Execute(L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='MarketItem' AND xtype='U') "
            L"CREATE TABLE MarketItem ("
            L"MarketItemId INT IDENTITY(1,1) PRIMARY KEY, "
            L"ItemId INT NOT NULL, "
            L"SellerId INT NOT NULL, "
            L"Price INT NOT NULL, "
            L"FOREIGN KEY (ItemId) REFERENCES ItemDB(ItemId) "
            L"ON DELETE CASCADE, "
            L"FOREIGN KEY (SellerId) REFERENCES UserStatus(UserId) "
            L"ON DELETE NO ACTION)"));
        LOG_EVENT(Color::GREEN, L"[DB] Table 'MarketItem' created or already exists.\n");


		GDBConnectionPool->Push(dbConn);
    }

    // 데이터 로드
    LoadData();
}

void DBManager::LoadData() {
    DBConnection* dbConn = GDBConnectionPool->Pop();
    ASSERT_CRASH(dbConn != nullptr);

    LOG_EVENT(Color::WHITE, L"[DB] Loading data from database...\n");
    // UserStatus 데이터 로드
    {
        DBBind<0, 3> dbBind(*dbConn, L"SELECT UserId, Username, GoldAmount FROM UserStatus");

        int32 userId = 0;
        WCHAR username[50] = { 0 };
        int32 goldAmount = 0;

        dbBind.BindCol(0, OUT userId);
        dbBind.BindCol(1, OUT username);
        dbBind.BindCol(2, OUT goldAmount);

        ASSERT_CRASH(dbBind.Execute());

        while (dbConn->Fetch()) {
            auto userStatus = make_shared<UserStatus>();
            userStatus->SetUserId(userId);
            userStatus->SetUsername(std::wstring(username));
            userStatus->SetGoldAmount(goldAmount);

            AddUserStatus(userStatus);
        }
    }

    // ItemDB 데이터 로드
    {
        DBBind<0, 7> dbBind(*dbConn, L"SELECT ItemId, TemplateId, Count, Slot, Equipped, IsOnMarket, UserId FROM ItemDB");

        int32 itemId = 0, templateId = 0, count = 0, slot = 0, userId = 0;
        bool equipped = false, isOnMarket = false;

        dbBind.BindCol(0, OUT itemId);
        dbBind.BindCol(1, OUT templateId);
        dbBind.BindCol(2, OUT count);
        dbBind.BindCol(3, OUT slot);
        dbBind.BindCol(4, OUT equipped);
        dbBind.BindCol(5, OUT isOnMarket);
        dbBind.BindCol(6, OUT userId);

        ASSERT_CRASH(dbBind.Execute());

        while (dbConn->Fetch()) {
            auto item = make_shared<ItemDB>();
            item->SetItemId(itemId);
            item->SetTemplateId(templateId);
            item->SetCount(count);
            item->SetSlot(slot);
            item->SetEquipped(equipped);
            item->SetOnMarket(isOnMarket);

            if (auto userStatus = GetUserStatus(userId)) {
                item->SetUserStatus(userStatus);
                userStatus->AddItem(item);
            }

            AddItem(item);
        }
    }

    // MarketItem 데이터 로드
    {
        DBBind<0, 4> dbBind(*dbConn, L"SELECT MarketItemId, ItemId, SellerId, Price FROM MarketItem");

        int32 marketItemId = 0, itemId = 0, sellerId = 0, price = 0;

        dbBind.BindCol(0, OUT marketItemId);
        dbBind.BindCol(1, OUT itemId);
        dbBind.BindCol(2, OUT sellerId);
        dbBind.BindCol(3, OUT price);

        ASSERT_CRASH(dbBind.Execute());

        while (dbConn->Fetch()) {
            auto marketItem = make_shared<MarketItem>();
            marketItem->SetMarketItemId(marketItemId);
            marketItem->SetItemId(itemId);
            marketItem->SetSellerId(sellerId);
            marketItem->SetPrice(price);

            if (auto item = GetItem(itemId)) {
                marketItem->SetItem(item);
            }

            if (auto seller = GetUserStatus(sellerId)) {
                marketItem->SetSeller(seller);
            }

            AddMarketItem(marketItem);
        }
    }
    LOG_EVENT(Color::WHITE, L"[DB] DB Data loading completed.\n");
    GDBConnectionPool->Push(dbConn);
}

