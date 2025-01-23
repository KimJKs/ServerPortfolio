#pragma once

#include "DataStructures.h"

class Player;

// IEffect �������̽�
class IEffect {
public:
    virtual ~IEffect() = default;
    virtual void Apply(std::shared_ptr<Player> player) = 0;
    virtual void Remove(std::shared_ptr<Player> player) = 0;
};

// HealEffect Ŭ����
class HealEffect : public IEffect {
public:
    HealEffect(int32 healAmount);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int32 _healAmount;
};

// AttackBoostEffect Ŭ����
class AttackBoostEffect : public IEffect {
public:
    AttackBoostEffect(int32 attackBoost);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int _attackBoost;
};

// DefenseBoostEffect Ŭ����
class DefenseBoostEffect : public IEffect {
public:
    DefenseBoostEffect(int32 defenseBoost);
    void Apply(std::shared_ptr<Player> player) override;
    void Remove(std::shared_ptr<Player> player) override;

private:
    int _defenseBoost;
};

// EffectFactory Ŭ����
class EffectFactory {
public:
    static std::shared_ptr<IEffect> CreateEffect(EffectData effectData);
};
