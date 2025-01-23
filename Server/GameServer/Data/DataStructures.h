#pragma once

#include "pch.h"
#include "DataManager.h"
#include <nlohmann/json.hpp>

struct EffectData {
    Protocol::EffectType effectType;
    int32 value = 0;

    static Protocol::EffectType GetEffectTypeByString(const std::string& effectString) {
        if (effectString == "AttackBoost") {
            return Protocol::EffectType::ATTACK_BOOST;
        }
        else if (effectString == "DefenseBoost") {
            return Protocol::EffectType::DEFENSE_BOOST;
        }
        else if (effectString == "Heal") {
            return Protocol::EffectType::HEAL;
        }
        return Protocol::EffectType::HEAL;
    }
};

struct ItemData {
    virtual ~ItemData() = default;
    int32 templateId = 0;
    std::string name;
    Protocol::ItemType itemType;
    std::vector<EffectData> effects;
};

struct WeaponData : public ItemData {
    Protocol::WeaponType weaponType;
};

struct ArmorData : public ItemData {
    Protocol::ArmorType armorType;
};

struct ConsumableData : public ItemData {
    Protocol::ConsumableType consumableType;
};

struct RewardData {
    int32 probability = 0;
    int32 itemId = 0;
    int32 count = 0;
};

struct GoldRewardData {
    int32 minGold = 0;
    int32 maxGold = 0;
};

struct MonsterData {
    int32 id = 0;
    std::string name;
    Protocol::StatInfo stat;
    GoldRewardData goldReward;
    std::vector<RewardData> rewards;
};

class StatDataLoader : public ILoader<int32, Protocol::StatInfo> {
public:
    std::vector<Protocol::StatInfo> stats;

    std::unordered_map<int32, Protocol::StatInfo> MakeDict() override {
        std::unordered_map<int32, Protocol::StatInfo> dict;
        for (auto& stat : stats) {
            stat.set_hp(stat.maxhp());

            stat.set_mp(stat.maxmp());
            dict[stat.level()] = stat;

        }
        return dict;
    }

    void FromJson(const nlohmann::json& json) {
        for (const auto& statJson : json["stats"]) {
            Protocol::StatInfo stat;
            stat.set_level(std::stoi(statJson.at("level").get<std::string>()));
            stat.set_maxhp(std::stoi(statJson.at("maxHp").get<std::string>()));
            stat.set_maxmp(std::stoi(statJson.at("maxMp").get<std::string>()));
            stat.set_attack(std::stoi(statJson.at("attack").get<std::string>()));
            stat.set_speed(std::stof(statJson.at("speed").get<std::string>()));
            stat.set_totalexp(std::stoi(statJson.at("totalExp").get<std::string>()));
            stat.set_defense(std::stoi(statJson.at("defense").get<std::string>()));
            stat.set_hp(stat.maxhp());
            stat.set_mp(stat.maxmp());
            stats.push_back(stat);
        }
    }
};

class ItemLoader : public ILoader<int32, ItemData> {
public:
    std::vector<WeaponData> weapons;
    std::vector<ArmorData> armors;
    std::vector<ConsumableData> consumables;

    std::unordered_map<int32, ItemData> MakeDict() override {
        std::unordered_map<int32, ItemData> dict;

        for (auto& item : weapons) {
            item.itemType = Protocol::ItemType::ITEM_TYPE_WEAPON;
            dict[item.templateId] = item;
        }
        for (auto& item : armors) {
            item.itemType = Protocol::ItemType::ITEM_TYPE_ARMOR;
            dict[item.templateId] = item;
        }
        for (auto& item : consumables) {
            item.itemType = Protocol::ItemType::ITEM_TYPE_CONSUMABLE;
            dict[item.templateId] = item;
        }

        return dict;
    }

    void FromJson(const nlohmann::json& json) {
        for (const auto& weaponJson : json["weapons"]) {
            WeaponData weapon;
            weapon.templateId = std::stoi(weaponJson.at("id").get<std::string>());
            weapon.name = weaponJson.at("name").get<std::string>();
            std::string weaponTypeString = weaponJson.at("weaponType").get<std::string>();

            if (weaponTypeString == "Sword") {
                weapon.weaponType = Protocol::WEAPON_TYPE_SWORD;
            }
            else if (weaponTypeString == "Bow") {
                weapon.weaponType = Protocol::WEAPON_TYPE_BOW;
            }
            else {
                weapon.weaponType = Protocol::WEAPON_TYPE_NONE;
            }

            for (const auto& effectJson : weaponJson.at("effects")) {
                EffectData effect;
                effect.effectType = EffectData::GetEffectTypeByString(effectJson.at("effectType").get<std::string>());
                effect.value = std::stoi(effectJson.at("value").get<std::string>());
                weapon.effects.push_back(effect);
            }

            weapons.push_back(weapon);
        }

        for (const auto& armorJson : json["armors"]) {
            ArmorData armor;
            armor.templateId = std::stoi(armorJson.at("id").get<std::string>());
            armor.name = armorJson.at("name").get<std::string>();
            std::string armorTypeString = armorJson.at("armorType").get<std::string>();

            if (armorTypeString == "Armor") {
                armor.armorType = Protocol::ARMOR_TYPE_ARMOR;
            }
            else if (armorTypeString == "Helmet") {
                armor.armorType = Protocol::ARMOR_TYPE_HELMET;
            }
            else {
                armor.armorType = Protocol::ARMOR_TYPE_NONE;
            }

            for (const auto& effectJson : armorJson.at("effects")) {
                EffectData effect;
                effect.effectType = EffectData::GetEffectTypeByString(effectJson.at("effectType").get<std::string>());
                effect.value = std::stoi(effectJson.at("value").get<std::string>());
                armor.effects.push_back(effect);
            }

            armors.push_back(armor);
        }

        for (const auto& consumableJson : json["consumables"]) {
            ConsumableData consumable;
            consumable.templateId = std::stoi(consumableJson.at("id").get<std::string>());
            consumable.name = consumableJson.at("name").get<std::string>();
            std::string consumableTypeString = consumableJson.at("consumableType").get<std::string>();

            if (consumableTypeString == "Potion") {
                consumable.consumableType = Protocol::CONSUMABLE_TYPE_POTION;
            }
            else {
                consumable.consumableType = Protocol::CONSUMABLE_TYPE_NONE;
            }

            for (const auto& effectJson : consumableJson.at("effects")) {
                EffectData effect;
                effect.effectType = EffectData::GetEffectTypeByString(effectJson.at("effectType").get<std::string>());
                effect.value = std::stoi(effectJson.at("value").get<std::string>());
                consumable.effects.push_back(effect);
            }

            consumables.push_back(consumable);
        }
    }
};

class MonsterLoader : public ILoader<int32, MonsterData> {
public:
    std::vector<MonsterData> monsters;

    std::unordered_map<int32, MonsterData> MakeDict() override {
        std::unordered_map<int32, MonsterData> dict;

        for (auto& monster : monsters) {
            dict[monster.id] = monster;
        }

        return dict;
    }

    void FromJson(const nlohmann::json& json) {
        for (const auto& monsterJson : json["monsters"]) {
            MonsterData monster;
            monster.id = std::stoi(monsterJson.at("id").get<std::string>());
            monster.name = monsterJson.at("name").get<std::string>();

            const auto& statJson = monsterJson.at("stat");
            monster.stat.set_level(std::stoi(statJson.at("level").get<std::string>()));
            monster.stat.set_maxhp(std::stoi(statJson.at("maxHp").get<std::string>()));
            monster.stat.set_hp(monster.stat.maxhp());
            monster.stat.set_maxmp(std::stoi(statJson.at("maxMp").get<std::string>()));
            monster.stat.set_mp(monster.stat.maxmp());
            monster.stat.set_attack(std::stoi(statJson.at("attack").get<std::string>()));
            monster.stat.set_speed(std::stof(statJson.at("speed").get<std::string>()));
            monster.stat.set_totalexp(std::stoi(statJson.at("totalExp").get<std::string>()));
            monster.stat.set_defense(std::stoi(statJson.at("defense").get<std::string>()));

            const auto& goldRewardJson = monsterJson.at("goldReward");
            monster.goldReward.minGold = std::stoi(goldRewardJson.at("minGold").get<std::string>());
            monster.goldReward.maxGold = std::stoi(goldRewardJson.at("maxGold").get<std::string>());

            for (const auto& rewardJson : monsterJson.at("rewards")) {
                RewardData reward;
                reward.probability = std::stoi(rewardJson.at("probability").get<std::string>());
                reward.itemId = std::stoi(rewardJson.at("itemId").get<std::string>());
                reward.count = std::stoi(rewardJson.at("count").get<std::string>());
                monster.rewards.push_back(reward);
            }

            monsters.push_back(monster);
        }
    }
};

struct MapData {
    int MapId = 0;                      // 맵 ID
    int Width = 0;                      // 맵의 가로 크기
    int Height = 0;                     // 맵의 세로 크기
    std::vector<std::string> Rows;      // 행 데이터 (2D 맵 데이터)
};

class MapDataLoader : public ILoader<int32, MapData> {
public:
    std::unordered_map<int32, MapData> mapDataDict; // 여러 맵 데이터를 저장

    // 맵 데이터를 사전 형태로 반환
    std::unordered_map<int32, MapData> MakeDict() override {
        return mapDataDict;
    }

    // JSON 데이터에서 여러 맵 로드
    void FromJson(const nlohmann::json& json) {
        for (const auto& mapJson : json["maps"]) {
            MapData map;
            map.MapId = mapJson.at("MapId").get<int>();
            map.Width = mapJson.at("Width").get<int>();
            map.Height = mapJson.at("Height").get<int>();

            if (mapJson.contains("Rows")) {
                map.Rows = mapJson.at("Rows").get<std::vector<std::string>>();
            }

            mapDataDict[map.MapId] = map;
        }
    }
};

struct SkillData {

    int32 skillId = 0;   
    std::string type;
    std::string name;              
    std::string description;       
    int32 cooldown;                 
    int32 cost;
    int32 damage;
};

struct Area : public SkillData {
    float range = 0.0f;      
    float duration = 0.0f;         
};

struct Directional : public SkillData {
    float angle = 0.0f;
    float range = 0.0f;
};

class SkillDataLoader : public ILoader<int32, std::shared_ptr<SkillData>> {
public:
    std::vector<std::shared_ptr<Area>> areaSkills;
    std::vector<std::shared_ptr<Directional>> directionalSkills;

    std::unordered_map<int32, std::shared_ptr<SkillData>> MakeDict() override {
        std::unordered_map<int32, std::shared_ptr<SkillData>> dict;

        // Area Skills
        for (auto& skill : areaSkills) {
            dict[skill->skillId] = std::static_pointer_cast<SkillData>(skill);
        }

        // Directional Skills
        for (auto& skill : directionalSkills) {
            dict[skill->skillId] = std::static_pointer_cast<SkillData>(skill);
        }

        return dict;
    }

    void FromJson(const nlohmann::json& json) {
        for (const auto& areaJson : json["areaSkills"]) {
            auto areaSkill = std::make_shared<Area>();
            areaSkill->skillId = areaJson.at("id").get<int>();
            areaSkill->type = areaJson.at("type").get<std::string>();
            areaSkill->name = areaJson.at("name").get<std::string>();
            areaSkill->description = areaJson.at("description").get<std::string>();
            areaSkill->cooldown = areaJson.at("cooldown").get<int>();
            areaSkill->damage = areaJson.at("damage").get<int>();
            areaSkill->cost = areaJson.at("cost").get<int>();
            areaSkill->range = areaJson.at("range").get<float>();
            areaSkill->duration = areaJson.at("duration").get<float>();

            areaSkills.push_back(areaSkill);
        }

        for (const auto& directionalJson : json["directionalSkills"]) {
            auto directionalSkill = std::make_shared<Directional>();
            directionalSkill->skillId = directionalJson.at("id").get<int>();
            directionalSkill->type = directionalJson.at("type").get<std::string>();
            directionalSkill->name = directionalJson.at("name").get<std::string>();
            directionalSkill->description = directionalJson.at("description").get<std::string>();
            directionalSkill->cooldown = directionalJson.at("cooldown").get<int>();
            directionalSkill->damage = directionalJson.at("damage").get<int>();
            directionalSkill->cost = directionalJson.at("cost").get<int>();
            directionalSkill->angle = directionalJson.at("angle").get<float>();
            directionalSkill->range = directionalJson.at("range").get<float>();

            directionalSkills.push_back(directionalSkill);
        }
    }
};