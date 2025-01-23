#pragma once
#include "Item.h"
#include <optional>

class Inventory {
public:
    void LoadPlayerItems(int userId, shared_ptr<Player> myPlayer);

    void Add(std::shared_ptr<Item> item);
    std::shared_ptr<Item> Get(int itemDbId) const;
    std::shared_ptr<Item> Find(const std::function<bool(std::shared_ptr<Item>)>& condition) const;
    void Change(std::shared_ptr<Item> item);
    void Remove(int itemDbId);
    std::optional<int> GetEmptySlot() const;

    std::shared_ptr<Item> UnequipItem(Protocol::ItemType type);

private:
    std::unordered_map<int, std::shared_ptr<Item>> items;
    std::unordered_map<Protocol::ItemType, std::shared_ptr<Item>> equippedItems;
};
