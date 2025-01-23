#pragma once

// Interface for loading data
template <typename Key, typename Value>
class ILoader {
public:
    virtual ~ILoader() = default;
    virtual std::unordered_map<Key, Value> MakeDict() = 0;
};

// Forward declarations for data structures
struct ItemData;
struct MonsterData;
struct MapData;
struct SkillData;

class DataManager {
public:
    static void LoadData();

    static const std::unordered_map<int, Protocol::StatInfo>& GetStatDict() { return StatDict; }
    static const std::unordered_map<int, ItemData>& GetItemDict() { return ItemDict; }
    static const std::unordered_map<int, MonsterData>& GetMonsterDict() { return MonsterDict; }
    static const std::unordered_map<int, MapData>& GetMapDict() { return MapDict; }
    static const std::unordered_map<int, std::shared_ptr<SkillData>>& GetSkillDict() { return SkillDict; }

private:
    template <typename Loader, typename Key, typename Value>
    static std::shared_ptr<Loader> LoadJson(const std::string& path);

    static std::unordered_map<int, Protocol::StatInfo> StatDict;
    static std::unordered_map<int, ItemData> ItemDict;
    static std::unordered_map<int, MonsterData> MonsterDict;
    static std::unordered_map<int, MapData> MapDict; 
    static std::unordered_map<int, std::shared_ptr<SkillData>> SkillDict;
};

