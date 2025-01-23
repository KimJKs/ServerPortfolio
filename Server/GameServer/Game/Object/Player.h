#pragma once

#include "Creature.h"
#include "Inventory.h"

class GameSession;

class Player : public Creature {
public:
    Player();
    virtual ~Player();

    const weak_ptr<GameSession>& GetSession() const { return _session; }

    void SetSession(const weak_ptr<GameSession>& value) { _session = value; }


    Inventory& GetInventory() { return _inventory; } 
    const Inventory& GetInventory() const { return _inventory; } 

public:
    bool isDummy = false;
    std::unordered_set<std::shared_ptr<Object>> visibleObjects;

private:
    weak_ptr<GameSession> _session;
    Inventory _inventory; // Inventory 멤버 변수 추가



};
