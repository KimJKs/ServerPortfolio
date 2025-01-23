#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include "DB_Class.h"

class DBManager
{
public:

    void Init();

    void LoadData(); // DB 데이터 로드


    void AddUserStatus(const std::shared_ptr<UserStatus>& userStatus) {
        userStatuses[userStatus->GetUserId()] = userStatus;
    }
    std::shared_ptr<UserStatus> GetUserStatus(int userId) {
        auto it = userStatuses.find(userId);
        return (it != userStatuses.end()) ? it->second : nullptr;
    }

    void AddItem(const std::shared_ptr<ItemDB>& item) {
        items[item->GetItemId()] = item;
    }
    std::shared_ptr<ItemDB> GetItem(int itemId) {
        auto it = items.find(itemId);
        return (it != items.end()) ? it->second : nullptr;
    }

    void AddMarketItem(const std::shared_ptr<MarketItem>& marketItem) {
        marketItems[marketItem->GetMarketItemId()] = marketItem;
    }
    std::shared_ptr<MarketItem> GetMarketItem(int marketItemId) {
        auto it = marketItems.find(marketItemId);
        return (it != marketItems.end()) ? it->second : nullptr;
    }

public:
    std::unordered_map<int, std::shared_ptr<UserStatus>> userStatuses;
    std::unordered_map<int, std::shared_ptr<ItemDB>> items;
    std::unordered_map<int, std::shared_ptr<MarketItem>> marketItems;
};
extern shared_ptr<DBManager> GDBManager;
