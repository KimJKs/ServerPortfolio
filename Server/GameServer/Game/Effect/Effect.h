#pragma once

#include "DataStructures.h"

class Player;

// IEffect 인터페이스
class IEffect {
public:
    virtual ~IEffect() = default;
    virtual void Apply(std::shared_ptr<Player> player) = 0;
    virtual void Remove(std::shared_ptr<Player> player) = 0;
};

// HealEffect 클래스
class HealEffect : public IEffect {
public:
    HealEffect(int32 healAmount);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int32 _healAmount;
};

// AttackBoostEffect 클래스
class AttackBoostEffect : public IEffect {
public:
    AttackBoostEffect(int32 attackBoost);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int _attackBoost;
};

// DefenseBoostEffect 클래스
class DefenseBoostEffect : public IEffect {
public:
    DefenseBoostEffect(int32 defenseBoost);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int _defenseBoost;
};

// EffectFactory 클래스
class EffectFactory {
public:
    static std::shared_ptr<IEffect> CreateEffect(EffectData effectData);
};
