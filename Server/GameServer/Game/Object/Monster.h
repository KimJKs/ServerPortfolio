#pragma once

#include "Creature.h"
#include <memory>
#include <chrono>
#include <random>
#include <cmath>
#include "Vector3.h"


class Player;

enum class MonsterState {
	Idle,
	Patrolling,
	Chasing,
	Attacking,
	Dead
};

class Monster : public Creature {
public:
	Monster();
	virtual ~Monster();

	void Init(int templateId);
	void Update() override;

	void OnDead(std::shared_ptr<Object> attacker) override;
	RewardData* GetRandomReward();
	void Respawn();

	const Vector3& GetSpawnPos() const { return _spawnPos; }

	void SetSpawnPos(const Vector3& value) { _spawnPos = value; _currentTargetPos = _spawnPos; }

private:
	void ChangeState(MonsterState newState);
	void UpdateIdle();
	void UpdatePatrolling();
	void UpdateChasing();
	void UpdateAttacking();

	void SetNewPatrolTarget();
	void MoveTowards(const Vector3& targetPos);

	MonsterState _currentState = MonsterState::Idle;
	std::weak_ptr<Player> _target;
	Vector3 _spawnPos;
	Vector3 _currentTargetPos;
	Vector3 _chasingTargetPos;

	float _aggroRange = 3.0f;
	float _attackRange = 1;
	std::chrono::steady_clock::time_point _lastPatrolTime;
	std::chrono::steady_clock::time_point _lastAttackTime;

	MonsterData monsterData;

	std::queue<Vector3> _path; // A* 경로를 저장

};
