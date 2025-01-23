#include "pch.h"
#include "DataManager.h"
#include <fstream> // ifstream ����� ���� �ʿ�
#include <nlohmann/json.hpp>
#include "DataStructures.h"


std::unordered_map<int, Protocol::StatInfo> DataManager::StatDict;
std::unordered_map<int, ItemData> DataManager::ItemDict;
std::unordered_map<int, MonsterData> DataManager::MonsterDict;
std::unordered_map<int, MapData> DataManager::MapDict;
std::unordered_map<int, std::shared_ptr<SkillData>> DataManager::SkillDict;

void DataManager::LoadData() {
    // ������ �����͸� JSON���� �о���� ����(dict) ���·� ��ȯ
    StatDict = LoadJson<StatDataLoader, int, Protocol::StatInfo>("StatData")->MakeDict();
    ItemDict = LoadJson<ItemLoader, int, ItemData>("ItemData")->MakeDict();
    MonsterDict = LoadJson<MonsterLoader, int, MonsterData>("MonsterData")->MakeDict();
    MapDict = LoadJson<MapDataLoader, int, MapData>("MapData")->MakeDict();
    SkillDict = LoadJson<SkillDataLoader, int, std::shared_ptr<SkillData>>("SkillData")->MakeDict();
    
    LOG_EVENT(Color::WHITE, L"[DataManager] Json Data load completed\n");
}

template <typename Loader, typename Key, typename Value>
std::shared_ptr<Loader> DataManager::LoadJson(const std::string& path) {
    // JSON ���� ����
    std::ifstream file(std::string(DATA_PATH) + path + ".json");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    nlohmann::json json;
    file >> json;
    file.close();

    // �δ� ��ü ���� �� JSON �����͸� �ε�
    auto loader = std::make_shared<Loader>();
    loader->FromJson(json); // Loader�� JSON �����͸� ó���ϵ��� �޼��� ȣ��
    return loader;
}
