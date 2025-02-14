#include "pch.h"
#include "DBManager.h"
#include "Monster.h"
#include "Player.h"
#include "Room.h"
#include <iostream>
#include "DataManager.h"
#include "DataStructures.h"
#include "DB_AccountService.h"
#include "DB_ItemService.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"
#include "DBWorker.h"
#include "RoomManager.h"

Monster::Monster() {
	SetObjectType(Protocol::ObjectType::MONSTER);

}

Monster::~Monster() = default;

void Monster::Init(int templateId) {

	MonsterData monsterD = DataManager::GetMonsterDict().at(templateId);
	monsterData = monsterD;
	Protocol::StatInfo stat;
	SetStat(monsterData.stat);

	_currentTargetPos = _spawnPos;
	ChangeState(MonsterState::Patrolling);

	_lastPatrolTime = std::chrono::steady_clock::now();
	_lastAttackTime = std::chrono::steady_clock::now();

	_objectInfo.set_name(monsterData.name);
	SetCreatureState(Protocol::CreatureState::IDLE);

	SetPosition(GetSpawnPos());
}

void Monster::Update() {
	switch (_currentState) {
	case MonsterState::Idle:
		UpdateIdle();
		break;
	case MonsterState::Patrolling:
		UpdatePatrolling();
		break;
	case MonsterState::Chasing:
		UpdateChasing();
		break;
	case MonsterState::Attacking:
		UpdateAttacking();
		break;
	case MonsterState::Dead:
		// Dead state requires no updates
		break;
	}
}

void Monster::OnDead(std::shared_ptr<Object> attacker)
{
	ChangeState(MonsterState::Dead);
	Creature::OnDead(attacker);

	// 몬스터 리스폰 처리 (3초 후 부활)
	if (auto room = GetRoom())
	{
		room->DoTimer(3000, std::bind(&Monster::Respawn, static_pointer_cast<Monster>(shared_from_this())));
	}

	if (!attacker)
		return;

	std::shared_ptr<Player> attackedPlayer = std::dynamic_pointer_cast<Player>(attacker);
	if (!attackedPlayer)
		return;

	// 보상 골드 랜덤 계산
	int32 minRand = monsterData.goldReward.minGold;
	int32 maxRand = monsterData.goldReward.maxGold;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(minRand, maxRand);
	int32 rewardGold = distrib(gen);

	int32 userDbId = attackedPlayer->GetSession().lock()->GetUserStatus()->GetUserId();

	RewardData* rewardData = GetRandomReward();

	// DBWorker에서 골드 지급 및 아이템 추가를 비동기 처리
	GDBWorker->AddJob([this, userDbId, rewardGold, attackedPlayer, rewardData]()
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			ASSERT_CRASH(dbConn != nullptr);

			DB_AccountService::ChangeGold(dbConn, userDbId, rewardGold, attackedPlayer->GetRoom());

			std::optional<int> slot;

			// 아이템 보상 처리
			if (rewardData)
			{
				slot = attackedPlayer->GetInventory().GetEmptySlot();
				if (slot.has_value())
				{
					int32 itemDbId = 0;
					DB_ItemService::AddItem(userDbId, rewardData->itemId, rewardData->count, slot.value(), itemDbId);

					// 후속 처리 (워크 스레드에서 실행)
					GRoomManager->DoAsync([attackedPlayer, rewardData, itemDbId, slot]()
						{
							if (!attackedPlayer)
								return;

							Protocol::S_ADD_ITEM packet;
							Protocol::ItemInfo* itemInfo = packet.add_items();
							itemInfo->set_count(rewardData->count);
							itemInfo->set_equipped(false);
							itemInfo->set_isonmarket(false);
							itemInfo->set_itemdbid(itemDbId);
							itemInfo->set_slot(slot.value());
							itemInfo->set_templateid(rewardData->itemId);

							shared_ptr<Item> newItem = (Item::CreateItem(*GDBManager->GetItem(itemDbId)));
							newItem->SetItemInfo(*itemInfo);

							attackedPlayer->GetInventory().Add(newItem);

							if (auto session = attackedPlayer->GetSession().lock()) {
								SEND_PACKET(packet);
							}
						}, true);
				}
			}

			GDBConnectionPool->Push(dbConn);
		});
}


RewardData* Monster::GetRandomReward()
{

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, 100);
	int rand = distrib(gen);

	int sum = 0;
	for (const auto& rewardData : monsterData.rewards)
	{
		sum += rewardData.probability;

		if (rand <= sum)
		{
			return new RewardData(rewardData); // 복사 생성자를 사용하여 새 RewardData 객체 생성 후 반환
		}
	}

	return nullptr;
}

void Monster::Respawn() {

	if (auto room = GetRoom()) {
		auto* quadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(room->GetRoomId());
		if (quadTree) {
			quadTree->Remove(std::dynamic_pointer_cast<Monster>(shared_from_this()));
		}
	}

	LOG_EVENT(Color::BLUE, L"[Monster %d] Respawned at spawn position.\n", GetId());
	ChangeState(MonsterState::Idle);
	SetCreatureState(Protocol::CreatureState::IDLE);
	SetPosition(_spawnPos);
	Protocol::StatInfo* statInfo = _objectInfo.mutable_statinfo();

	statInfo->set_hp(statInfo->maxhp());

	GetRoom()->Enter(std::dynamic_pointer_cast<Object>(shared_from_this()));


	if (auto room = GetRoom()) {
		auto* quadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(room->GetRoomId());
		if (quadTree) {
			quadTree->Insert(std::dynamic_pointer_cast<Monster>(shared_from_this()));
		}
	}

}

void Monster::ChangeState(MonsterState newState) {
	if (_currentState == newState) return;

	_currentState = newState;

	// 상태 변경 로그
	switch (newState) {
	case MonsterState::Idle:
		//LOG_EVENT(Color::WHITE, L"[Monster %d] State changed to: Idle\n", GetId());
		break;
	case MonsterState::Patrolling:
		//LOG_EVENT(Color::GREEN, L"[Monster %d] State changed to: Patrolling\n", GetId());
		break;
	case MonsterState::Chasing:
		//LOG_EVENT(Color::YELLOW, L"[Monster %d] State changed to: Chasing\n", GetId());
		break;
	case MonsterState::Attacking:
		//LOG_EVENT(Color::RED, L"[Monster %d] State changed to: Attacking\n", GetId());
		break;
	case MonsterState::Dead:
		//LOG_EVENT(Color::BLUE, L"[Monster %d] State changed to: Dead\n", GetId());
		break;
	}
}

void Monster::UpdateIdle() {
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - _lastPatrolTime).count() >= 0.5F) {
		ChangeState(MonsterState::Patrolling);
	}
}

void Monster::UpdatePatrolling() {
	if (Vector3::Distance(GetPosition(), _currentTargetPos) < 0.2f) {
		SetNewPatrolTarget();
	}
	else {
		MoveTowards(_currentTargetPos);
	}

	if (auto room = GetRoom()) {
		for (const auto& [id, player] : room->GetPlayers()) {
			if ((Vector3::Distance(GetPosition(), player->GetPosition())) <= _aggroRange) {
				_target = player;
				ChangeState(MonsterState::Chasing);
				break;
			}
		}
	}
}

void Monster::SetNewPatrolTarget() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-1.5f, 1.5f);

	_currentTargetPos = { _spawnPos.x + dist(gen), 0.0f, _spawnPos.z + dist(gen) };
	_lastPatrolTime = std::chrono::steady_clock::now();
}

void Monster::UpdateChasing() {
	auto target = _target.lock();
	if (!target || target->IsDead()) {
		LOG_EVENT(Color::RED, L"[Monster %d] Target lost or dead. Returning to patrol.\n", GetId());
		ChangeState(MonsterState::Patrolling);
		return;
	}

	float distance = (Vector3::Distance(GetPosition(), (target->GetPosition())));
	float spawnPosDistance = (Vector3::Distance(GetPosition(), (GetSpawnPos())));
	if (distance <= _attackRange) {
		LOG_EVENT(Color::GREEN, L"[Monster %d] Target in attack range. Switching to Attacking.\n", GetId());
		ChangeState(MonsterState::Attacking);
	}
	else if (distance > _aggroRange * 1.5f|| spawnPosDistance> _aggroRange*2) {
		LOG_EVENT(Color::RED, L"[Monster %d] Target out of aggro range. Returning to patrol.\n", GetId());
		ChangeState(MonsterState::Patrolling);
	}
	else {
		
		if (_path.empty() ) {
			if (auto room = GetRoom()) 
			{
				if (room->hasMapData || Vector3::Distance(_chasingTargetPos, target->GetPosition()) > 0.5f) {
					_path = room->GetPath(GetPosition(), target->GetPosition());
					LOG_EVENT(Color::GREEN, L"[Monster %d] Pathfinding success. Moving towards target.\n", GetId());
					_chasingTargetPos = target->GetPosition();
				}
				else
				{
					MoveTowards(target->GetPosition()); // 기존 방식으로 이동
				}
			}
		}

		if (!_path.empty()) {
			MoveTowards(_path.front());
		}
	}
}


void Monster::UpdateAttacking() {
	auto target = _target.lock();
	if (!target || target->IsDead()) {
		ChangeState(MonsterState::Patrolling);
		return;
	}

	float distance = (Vector3::Distance(GetPosition(), (target->GetPosition())));
	if (distance > _attackRange) {
		LOG_EVENT(Color::YELLOW, L"[Monster %d] Target moved out of attack range. Switching to Chasing.\n", GetId());
		ChangeState(MonsterState::Chasing);
	}
	else {
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - _lastAttackTime).count() >= 1.5) {
			LOG_EVENT(Color::RED, L"[Monster %d] Attacking target %d.\n", GetId(), target->GetId());
			Vector3 direction = (target->GetPosition() - GetPosition());
			direction.Normalize();

			Protocol::S_ATTACK attackPacket;
			attackPacket.set_attackerid(GetId());
			attackPacket.set_dirx(direction.x);
			attackPacket.set_dirz(direction.z);

			if (auto roomLock = GetRoom()) {
				roomLock->Broadcast(attackPacket,std::dynamic_pointer_cast<Creature>(shared_from_this()));

			}
			target->OnDamaged(GetStat().attack(), std::dynamic_pointer_cast<Player>(shared_from_this()));
			_lastAttackTime = now;
		}
	}
}

void Monster::MoveTowards(const Vector3& targetPos) {
	SetCreatureState(Protocol::CreatureState::MOVING);

	if (auto room = GetRoom()) {
		auto* quadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(room->GetRoomId());
		if (quadTree) {
			quadTree->Remove(std::dynamic_pointer_cast<Monster>(shared_from_this()));
		}
	}

	// A* 경로가 있으면 이를 따라 이동
	if (!_path.empty()) {
		Vector3 nextPos = _path.front(); // 경로의 다음 위치
		if ((Vector3::Distance(GetPosition(), (nextPos))) < 0.3f) {
			_path.pop(); // 현재 위치를 제거
			if (_path.empty()) {
				ChangeState(MonsterState::Idle); // 경로 종료 시 Idle 상태로 전환
				return;
			}
			nextPos = _path.front();
		}

		Vector3 direction = nextPos - GetPosition();
		direction.Normalize();
		Vector3 movePos = GetPosition() + direction * GetStat().speed() * 0.1f;
		SetPosition(movePos);
	}
	else {
		// 기존 방식으로 이동
		Vector3 direction = targetPos - GetPosition();
		direction = direction.Normalize();
		Vector3 movePos = GetPosition() + direction * GetStat().speed() * 0.1f;
		SetPosition(movePos);
	}

	if (auto room = GetRoom()) {
		auto* quadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(room->GetRoomId());
		if (quadTree) {
			quadTree->Insert(std::dynamic_pointer_cast<Monster>(shared_from_this()));
		}
	}

	// 이동 패킷 브로드캐스트
	Protocol::S_MOVE movePacket;
	movePacket.mutable_posinfo()->CopyFrom(GetPositionInfo());
	movePacket.set_objectid(GetId());
	if (auto roomLock = GetRoom()) {
		
		roomLock->Broadcast(movePacket, std::dynamic_pointer_cast<Creature>(shared_from_this()));
	}
}

