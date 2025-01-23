#include "pch.h"
#include "ObjectManager.h"
#include "Object.h"


bool ObjectManager::Remove(int objectId) {
    Protocol::ObjectType type = GetObjectTypeById(objectId);

    std::lock_guard<std::mutex> lock(_mutex);
    if (type == Protocol::ObjectType::PLAYER) {
        return _players.erase(objectId) > 0;
    }

    return false;
}

std::shared_ptr<Player> ObjectManager::FindPlayer(int objectId) {
    Protocol::ObjectType type = GetObjectTypeById(objectId);

    std::lock_guard<std::mutex> lock(_mutex);
    if (type == Protocol::ObjectType::PLAYER) {
        auto it = _players.find(objectId);
        if (it != _players.end()) {
            return it->second;
        }
    }

    return nullptr;
}

Protocol::ObjectType ObjectManager::GetObjectTypeById(int id) {
    int type = (id >> 24) & 0x7F;
    return static_cast<Protocol::ObjectType>(type);
}

int ObjectManager::GenerateId(Protocol::ObjectType type) {
    std::lock_guard<std::mutex> lock(_mutex);
    return (static_cast<int>(type) << 24) | (_counter++);
}


/*
// Usage example
int main() {
    auto& manager = ObjectManager::Instance();

    auto player1 = manager.Add<Player>();
    std::cout << "Player 1 ID: " << player1->Id << std::endl;

    auto foundPlayer = manager.FindPlayer(player1->Id);
    if (foundPlayer) {
        std::cout << "Found Player with ID: " << foundPlayer->Id << std::endl;
    }

    manager.Remove(player1->Id);
    auto removedPlayer = manager.FindPlayer(player1->Id);
    if (!removedPlayer) {
        std::cout << "Player with ID " << player1->Id << " removed successfully." << std::endl;
    }

    return 0;
}
*/