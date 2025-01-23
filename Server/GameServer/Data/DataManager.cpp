#include "pch.h"
#include "DataManager.h"
#include <fstream> // ifstream 사용을 위해 필요
#include <nlohmann/json.hpp>
#include "DataStructures.h"


std::unordered_map<int, Protocol::StatInfo> DataManager::StatDict;
std::unordered_map<int, ItemData> DataManager::ItemDict;
std::unordered_map<int, MonsterData> DataManager::MonsterDict;
std::unordered_map<int, MapData> DataManager::MapDict;
std::unordered_map<int, std::shared_ptr<SkillData>> DataManager::SkillDict;

void DataManager::LoadData() {
    // 각각의 데이터를 JSON에서 읽어오고 사전(dict) 형태로 변환
    StatDict = LoadJson<StatDataLoader, int, Protocol::StatInfo>("StatData")->MakeDict();
    ItemDict = LoadJson<ItemLoader, int, ItemData>("ItemData")->MakeDict();
    MonsterDict = LoadJson<MonsterLoader, int, MonsterData>("MonsterData")->MakeDict();
    MapDict = LoadJson<MapDataLoader, int, MapData>("MapData")->MakeDict();
    SkillDict = LoadJson<SkillDataLoader, int, std::shared_ptr<SkillData>>("SkillData")->MakeDict();
    
    LOG_EVENT(Color::WHITE, L"[DataManager] Json Data load completed\n");
}

template <typename Loader, typename Key, typename Value>
std::shared_ptr<Loader> DataManager::LoadJson(const std::string& path) {
    // JSON 파일 열기
    std::ifstream file(std::string(DATA_PATH) + path + ".json");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    nlohmann::json json;
    file >> json;
    file.close();

    // 로더 객체 생성 및 JSON 데이터를 로드
    auto loader = std::make_shared<Loader>();
    loader->FromJson(json); // Loader가 JSON 데이터를 처리하도록 메서드 호출
    return loader;
}
