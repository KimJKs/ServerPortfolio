#pragma once
#include "DataManager.h"
#include "Player.h"
#include "DB_Class.h"
#include "Effect.h"

class Player;

class Item {
public:
    Item();
    explicit Item(Protocol::ItemType type);
    virtual ~Item() = default;

    // Getters and Setters
    int32 GetItemDbId() const { return itemDbId; }
    void SetItemDbId(int32 dbId) { itemDbId = dbId; }

    int32 GetTemplateId() const { return templateId; }
    void SetTemplateId(int32 id) { templateId = id; }

    int32 GetCount() const { return count; }
    void SetCount(int32 cnt) { count = cnt; }

    int32 GetSlot() const { return slot; }
    void SetSlot(int32 s) { slot = s; }

    bool IsEquipped() const { return equipped; }
    void SetEquipped(bool eq) { equipped = eq; }

    bool IsOnMarket() const { return isOnMarket; }
    void SetOnMarket(bool market) { isOnMarket = market; }

    Protocol::ItemType GetItemType() const { return itemType; }
    bool IsStackable() const { return stackable; }
    bool IsEquipable() const { return equipable; }

    virtual void ApplyEffects(std::shared_ptr<Player> player);
    virtual void RemoveEffects(std::shared_ptr<Player> player);

    static std::shared_ptr<Item> CreateItem(const ItemDB& itemDb);


    Protocol::ItemInfo GetItemInfo() const;
    void SetItemInfo(const Protocol::ItemInfo& itemInfo);

protected:
    int32 itemDbId = 0;
    int32 templateId = 0;
    int32 count = 0;
    int32 slot = 0;
    bool equipped = false;
    bool isOnMarket = false;

    Protocol::ItemType itemType;
    bool stackable = false;
    bool equipable = false;

    std::vector<std::shared_ptr<IEffect>> effects;
};

class Weapon : public Item {
public:
    explicit Weapon(int32 templateId);
    void Init(int32 templateId);

private:
    Protocol::WeaponType weaponType;
    int32 damage = 0;
};

class Armor : public Item {
public:
    explicit Armor(int32 templateId);
    void Init(int32 templateId);

private:
    Protocol::ArmorType armorType;
    int32 defence = 0;
};

class Consumable : public Item {
public:
    explicit Consumable(int32 templateId);
    void Init(int32 templateId);

private:
    Protocol::ConsumableType consumableType;
    int32 maxCount = 0;
};
