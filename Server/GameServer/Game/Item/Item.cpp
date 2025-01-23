#include "pch.h"
#include "Item.h"
#include "DataStructures.h"

Item::Item()
{
}

Item::Item(Protocol::ItemType type) : itemType(type) {}

void Item::ApplyEffects(std::shared_ptr<Player> player) {
    for (const auto& effect : effects) {
        effect->Apply(player);
    }
}

void Item::RemoveEffects(std::shared_ptr<Player> player) {
    for (const auto& effect : effects) {
        effect->Remove(player);
    }
}

std::shared_ptr<Item> Item::CreateItem(const ItemDB& itemDb) {
    auto& itemData = DataManager::GetItemDict().at(itemDb.GetTemplateId());

    std::shared_ptr<Item> item;
    switch (itemData.itemType) {
    case Protocol::ItemType::ITEM_TYPE_WEAPON:
        item = std::make_shared<Weapon>(itemDb.GetTemplateId());
        break;
    case Protocol::ItemType::ITEM_TYPE_ARMOR:
        item = std::make_shared<Armor>(itemDb.GetTemplateId());
        break;
    case Protocol::ItemType::ITEM_TYPE_CONSUMABLE:
        item = std::make_shared<Consumable>(itemDb.GetTemplateId());
        break;
    default:
        return nullptr;
    }

    if (!item)
        return nullptr;

    item->SetItemDbId(itemDb.GetItemId());
    item->SetCount(itemDb.GetCount());
    item->SetSlot(itemDb.GetSlot());
    item->SetEquipped(itemDb.IsEquipped());
    item->SetOnMarket(itemDb.IsOnMarket());

    for (const auto& effectData : itemData.effects) {
        auto effect = EffectFactory::CreateEffect(effectData);
        if (effect)
            item->effects.push_back(effect);
    }

    return item;
}

 Protocol::ItemInfo Item::GetItemInfo()const
{
    Protocol::ItemInfo iteminfo;
    iteminfo.set_count(count);
    iteminfo.set_equipped(equipped);
    iteminfo.set_isonmarket(isOnMarket);
    iteminfo.set_itemdbid(itemDbId);
    iteminfo.set_slot(slot);
    iteminfo.set_templateid(templateId);

    return iteminfo;
}

void Item::SetItemInfo(const Protocol::ItemInfo& itemInfo)
{
    itemDbId = itemInfo.itemdbid();
    templateId = itemInfo.templateid();
    count = itemInfo.count();
    slot = itemInfo.slot();
    equipped = itemInfo.equipped();
    isOnMarket = itemInfo.isonmarket();
}

// Weapon Implementation
Weapon::Weapon(int templateId) : Item(Protocol::ItemType::ITEM_TYPE_WEAPON) {
    Init(templateId);
}

void Weapon::Init(int templateId) {
    const ItemData& itemData = DataManager::GetItemDict().at(templateId);
    const auto& data = static_cast<const WeaponData&>(itemData); // const 유지
    this->templateId = data.templateId;
    this->stackable = false;
    this->equipable = true;
    this->weaponType = data.weaponType;
}

// Armor Implementation
Armor::Armor(int templateId) : Item(Protocol::ItemType::ITEM_TYPE_ARMOR) {
    Init(templateId);
}

void Armor::Init(int templateId) {
    const ItemData& itemData = DataManager::GetItemDict().at(templateId);
    const auto& data = static_cast<const ArmorData&>(itemData); // const 유지
    this->templateId = data.templateId;
    this->stackable = false;
    this->equipable = true;
    this->armorType = data.armorType;
}

// Consumable Implementation
Consumable::Consumable(int templateId) : Item(Protocol::ItemType::ITEM_TYPE_CONSUMABLE) {
    Init(templateId);
}

void Consumable::Init(int templateId) {
    const ItemData& itemData = DataManager::GetItemDict().at(templateId);
    const auto& data = static_cast<const ConsumableData&>(itemData); // const 유지
    this->templateId = data.templateId;
    this->stackable = true;
    this->equipable = false;
    this->consumableType = data.consumableType;
}
