#include "pch.h"
#include "Effect.h"
#include "Player.h"
#include "GameSession.h"
#include "DataStructures.h"

// HealEffect 구현
HealEffect::HealEffect(int32 healAmount) : _healAmount(healAmount) {}

void HealEffect::Apply(std::shared_ptr<Player> player) {
    Protocol::StatInfo currentStat = player->GetStat();
    int32 newHp = min(currentStat.maxhp(), currentStat.hp() + _healAmount);
    currentStat.set_hp(newHp);
    player->SetStat(currentStat);

    Protocol::S_UPDATE_RESOURCE changePacket;
    changePacket.set_changevalue(_healAmount);
    changePacket.set_objectid(player->GetId());
    changePacket.set_nowvalue(newHp);
    changePacket.set_type(Protocol::ResourceType::HP);

    if (auto session = player->GetSession().lock()) {
        SEND_PACKET(changePacket);
    }
}

void HealEffect::Remove(std::shared_ptr<Player> player) {
    // Heal은 즉시 효과이므로 제거 작업은 필요 없음
}

// AttackBoostEffect 구현
AttackBoostEffect::AttackBoostEffect(int32 attackBoost) : _attackBoost(attackBoost) {}

void AttackBoostEffect::Apply(std::shared_ptr<Player> player) {
    Protocol::StatInfo currentStat = player->GetStat();
    currentStat.set_attack(currentStat.attack() + _attackBoost);
    player->SetStat(currentStat);
}

void AttackBoostEffect::Remove(std::shared_ptr<Player> player) {
    Protocol::StatInfo currentStat = player->GetStat();
    currentStat.set_attack(currentStat.attack() - _attackBoost);
    player->SetStat(currentStat);
}

// DefenseBoostEffect 구현
DefenseBoostEffect::DefenseBoostEffect(int32 defenseBoost) : _defenseBoost(defenseBoost) {}

void DefenseBoostEffect::Apply(std::shared_ptr<Player> player) {
    Protocol::StatInfo currentStat = player->GetStat();
    currentStat.set_defense(currentStat.defense() + _defenseBoost);
    player->SetStat(currentStat);
}

void DefenseBoostEffect::Remove(std::shared_ptr<Player> player) {
    Protocol::StatInfo currentStat = player->GetStat();
    currentStat.set_defense(currentStat.defense() - _defenseBoost);
    player->SetStat(currentStat);
}

// EffectFactory 구현
std::shared_ptr<IEffect> EffectFactory::CreateEffect(EffectData effectData) {
    switch (effectData.effectType) {
    case Protocol::EffectType::HEAL:
        return std::make_shared<HealEffect>(effectData.value);
    case Protocol::EffectType::ATTACK_BOOST:
        return std::make_shared<AttackBoostEffect>(effectData.value);
    case Protocol::EffectType::DEFENSE_BOOST:
        return std::make_shared<DefenseBoostEffect>(effectData.value);
    default:
        return nullptr;
    }
}
