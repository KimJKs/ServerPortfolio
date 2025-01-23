#include "pch.h"
#include "DB_ItemService.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"
#include "DB_Class.h"
#include "DBManager.h"


void DB_ItemService::AddItem(int32 userId, int32 templateId, int32 count, int32 slot, int32& itemDbId) {

    LOG_EVENT(Color::WHITE, L"[DB_ItemService] Adding item for userId=%d, templateId=%d, count=%d, slot=%d...\n", userId, templateId, count, slot);

    DBConnection* dbConn = GDBConnectionPool->Pop();
    ASSERT_CRASH(dbConn != nullptr);

    DBBind<4, 1> dbBind(*dbConn, L"INSERT INTO ItemDB (TemplateId, Count, UserId, Slot) OUTPUT INSERTED.ItemId VALUES (?, ?, ?, ?)");
    dbBind.BindParam(0, templateId);
    dbBind.BindParam(1, count);
    dbBind.BindParam(2, userId);
    dbBind.BindParam(3, slot);
    dbBind.BindCol(0, OUT itemDbId);

    ASSERT_CRASH(dbBind.Execute());
    while (dbConn->Fetch());

    // 캐싱된 데이터 업데이트
    auto newItem = std::make_shared<ItemDB>();
    newItem->SetItemId(itemDbId);
    newItem->SetTemplateId(templateId);
    newItem->SetCount(count);
    newItem->SetUserStatus(GDBManager->GetUserStatus(userId));
    GDBManager->AddItem(newItem);
 
    GDBConnectionPool->Push(dbConn);
}

void DB_ItemService::UpdateItem(int32 itemDbId, int32 count, bool equipped) {
    LOG_EVENT(Color::WHITE, L"[DB] Updating item. ItemId=%d, Count=%d, Equipped=%s...\n", itemDbId, count, equipped ? L"true" : L"false");

    DBConnection* dbConn = GDBConnectionPool->Pop();
    ASSERT_CRASH(dbConn != nullptr);

    DBBind<3, 0> dbBind(*dbConn, L"UPDATE ItemDB SET Count = ?, Equipped = ? WHERE ItemId = ?");
    dbBind.BindParam(0, count);
    dbBind.BindParam(1, equipped);
    dbBind.BindParam(2, itemDbId);

    ASSERT_CRASH(dbBind.Execute());

    // 캐싱된 데이터 업데이트
    if (auto item = GDBManager->GetItem(itemDbId)) {
        item->SetCount(count);
        item->SetEquipped(equipped);
    }

    GDBConnectionPool->Push(dbConn);
}

void DB_ItemService::RemoveItem(int32 itemDbId) {
    LOG_EVENT(Color::WHITE, L"[DB] Removing item. ItemId=%d...\n", itemDbId);

    DBConnection* dbConn = GDBConnectionPool->Pop();
    ASSERT_CRASH(dbConn != nullptr);

    DBBind<1, 0> dbBind(*dbConn, L"DELETE FROM ItemDB WHERE ItemId = ?");
    dbBind.BindParam(0, itemDbId);

    ASSERT_CRASH(dbBind.Execute());

    // 캐싱된 데이터 삭제
    GDBManager->marketItems.erase(itemDbId);

    GDBConnectionPool->Push(dbConn);
}
