#pragma once
#include "pch.h"

class ItemDB;
class MarketItem;

class UserStatus {
public:
    UserStatus() = default;
    ~UserStatus() = default;

    int32 GetUserId() const { return userId; }
    void SetUserId(int32 id) { userId = id; }

    const std::wstring& GetUsername() const { return username; }
    void SetUsername(const std::wstring& name) { username = name; }

    int32 GetGoldAmount() const { return goldAmount; }
    void SetGoldAmount(int32 amount) { goldAmount = amount; }

    bool IsLoggedIn() const { return isLoggedIn; }
    void SetLoggedIn(bool loggedIn) { isLoggedIn = loggedIn; }

    const std::vector<std::shared_ptr<ItemDB>>& GetItems() const { return items; }
    void AddItem(const std::shared_ptr<ItemDB>& item) { items.push_back(item); }

private:
    int32 userId;
    std::wstring username;
    int32 goldAmount;
    bool isLoggedIn;
    std::vector<std::shared_ptr<ItemDB>> items;
};

class ItemDB {
public:
    ItemDB() = default;
    ~ItemDB() = default;

    int32 GetItemId() const { return itemId; }
    void SetItemId(int32 id) { itemId = id; }

    int32 GetTemplateId() const { return templateId; }
    void SetTemplateId(int32 id) { templateId = id; }

    int32 GetCount() const { return count; }
    void SetCount(int32 c) { count = c; }

    int32 GetSlot() const { return slot; }
    void SetSlot(int32 s) { slot = s; }

    bool IsEquipped() const { return equipped; }
    void SetEquipped(bool eq) { equipped = eq; }

    bool IsOnMarket() const { return isOnMarket; }
    void SetOnMarket(bool market) { isOnMarket = market; }

    std::weak_ptr<UserStatus> GetUserStatus() const { return userStatus; }
    void SetUserStatus(const std::weak_ptr<UserStatus>& user) { userStatus = user; }

private:
    int32 itemId;
    int32 templateId;
    int32 count;
    int32 slot;
    bool equipped;
    bool isOnMarket;
    std::weak_ptr<UserStatus> userStatus;
};

class MarketItem {
public:
    MarketItem() = default;
    ~MarketItem() = default;

    int32 GetMarketItemId() const { return marketItemId; }
    void SetMarketItemId(int32 id) { marketItemId = id; }

    int32 GetItemId() const { return itemId; }
    void SetItemId(int32 id) { itemId = id; }

    std::shared_ptr<ItemDB> GetItem() const { return item; }
    void SetItem(const std::shared_ptr<ItemDB>& i) { item = i; }

    int32 GetSellerId() const { return sellerId; }
    void SetSellerId(int32 id) { sellerId = id; }

    std::shared_ptr<UserStatus> GetSeller() const { return seller; }
    void SetSeller(const std::shared_ptr<UserStatus>& s) { seller = s; }

    int32 GetPrice() const { return price; }
    void SetPrice(int32 p) { price = p; }

private:
    int32 marketItemId;
    int32 itemId;
    std::shared_ptr<ItemDB> item;
    int32 sellerId;
    std::shared_ptr<UserStatus> seller;
    int32 price;
};
