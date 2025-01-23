#include "pch.h"
#include "Inventory.h"
#include "DBManager.h"


void Inventory::LoadPlayerItems(int userId, shared_ptr<Player> myPlayer)
{
	Protocol::S_ITEM_LIST itemListPacket;

	auto& itemsFromDb = GDBManager->items;

	std::vector<std::shared_ptr<ItemDB>> filteredItems;
	for (const auto& [key, itemDb] : itemsFromDb) {
		if (itemDb->GetUserStatus().lock()->GetUserId() == userId) {
			filteredItems.push_back(itemDb);
		}
	}

	for (const auto& itemDb : filteredItems) {
		// Item ����
		std::shared_ptr<Item> item = Item::CreateItem(*itemDb);
		if (item) {
			Add(item);
			// ItemInfo ���� �� ������ ����
			Protocol::ItemInfo* info = itemListPacket.add_items();
			info->MergeFrom(item->GetItemInfo());


			// �������� ���� ���¶�� ȿ�� ����
			if (itemDb->IsEquipped()) {
				item->ApplyEffects(myPlayer);
			}
		}
	}
	if (auto session = myPlayer->GetSession().lock()) {
		SEND_PACKET(itemListPacket);
	}
}

void Inventory::Add(std::shared_ptr<Item> item) {
	if (item->IsEquipped())
		equippedItems[item->GetItemType()] = item;

	items[item->GetItemDbId()] = item;
}

std::shared_ptr<Item> Inventory::Get(int itemDbId) const {
	auto it = items.find(itemDbId);
	return (it != items.end()) ? it->second : nullptr;
}

std::shared_ptr<Item> Inventory::Find(const std::function<bool(std::shared_ptr<Item>)>& condition) const {
	for (const auto& [id, item] : items) {
		if (condition(item))
			return item;
	}
	return nullptr;
}

void Inventory::Change(std::shared_ptr<Item> item) {
	if (item->IsEquipped())
		equippedItems[item->GetItemType()] = item;

	items[item->GetItemDbId()] = item;
}

void Inventory::Remove(int itemDbId) {
	items.erase(itemDbId);
}

std::optional<int> Inventory::GetEmptySlot() const {
	for (int slot = 0; slot < 20; ++slot) {
		auto it = std::find_if(items.begin(), items.end(),
			[slot](const auto& pair) { return pair.second->GetSlot() == slot; });
		if (it == items.end())
			return slot;
	}
	return std::nullopt;
}

std::shared_ptr<Item> Inventory::UnequipItem(Protocol::ItemType type)
{
	auto it = equippedItems.find(type); // �ش� Ÿ���� �������� ã��
	if (it == equippedItems.end()) {
		return nullptr; // �ش� Ÿ���� �������� ������ nullptr ��ȯ
	}

	std::shared_ptr<Item> item = it->second; // ������ ������ ����
	equippedItems.erase(it); // ������ ����

	return item; // ������ ������ ��ȯ
}
