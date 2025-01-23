#include "pch.h"

#include "Room.h"
#include "Object.h"
#include "Monster.h"
#include "Player.h"
#include "DB_ItemService.h"
#include "DBManager.h"
#include "DBBind.h"
#include "Vector3.h"
#include "ObjectManager.h"
#include "QuadTree.h"



void Room::Update() {

	for (auto& [id, monster] : _monsters) {
		monster->Update();
	}

}

void Room::InitRoom()
{
	auto  mapData = DataManager::GetMapDict().at(_mapDataNumber);
	if (mapData.Rows.size() > 0)
	{
		hasMapData = true;
		_astar = std::make_shared<AStar>(mapData.Width, mapData.Height, mapData.Rows);
	}

}

void Room::InitMonsterInField()
{
	auto  mapData = DataManager::GetMapDict().at(_mapDataNumber);

	BoundingBox mapBounds(Vector3(-mapData.Width / 2, 0, -mapData.Height / 2), Vector3(mapData.Width / 2, 0, mapData.Height / 2));
	QuadTreeManager::GetInstance().InitializeRoom(_roomId, mapBounds, mapBounds);

	float centralMin = -10.0f;
	float centralMax = 10.0f;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(mapBounds.minPoint.x, mapBounds.maxPoint.x);
	std::uniform_real_distribution<float> distZ(mapBounds.minPoint.z, mapBounds.maxPoint.z);

	for (size_t i = 0; i < 0; ++i)
	{
		float x, z;

		do {
			x = distX(gen);
			z = distZ(gen);
		} while (x >= centralMin && x <= centralMax && z >= centralMin && z <= centralMax);

		Vector3 spawnPos(x, 0, z);

		auto monster = ObjectManager::Instance().Add<Monster>();
		monster->SetSpawnPos(spawnPos);
		monster->Init(1);
		monster->SetRoom(std::static_pointer_cast<Room>(shared_from_this()));

		Enter(monster);
		auto* newRoomPlayerQuadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(_roomId);

		newRoomPlayerQuadTree->Insert(monster);


	}
	LOG_EVENT(Color::GREEN, L"[Room] Monster entered [Field].\n");
}

void Room::InitMonsterSpawn(std::vector<Vector3> spawnZones)
{
	auto  mapData = DataManager::GetMapDict().at(_mapDataNumber);

	BoundingBox mapBounds(Vector3(-mapData.Width / 2, 0, -mapData.Height / 2), Vector3(mapData.Width / 2, 0, mapData.Height / 2));
	QuadTreeManager::GetInstance().InitializeRoom(_roomId, mapBounds, mapBounds);

	for (size_t i = 0; i < spawnZones.size(); ++i)
	{
		auto monster = ObjectManager::Instance().Add<Monster>();
		monster->SetSpawnPos(spawnZones[i]);
		monster->Init(1);
		monster->SetRoom(std::static_pointer_cast<Room>(shared_from_this()));
		Enter(monster);
		auto* newRoomPlayerQuadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(_roomId);

		newRoomPlayerQuadTree->Insert(monster);

		
	}
	LOG_EVENT(Color::GREEN, L"[Room] Monster entered [Dungeon].\n");
}

void Room::SendVisibleObjects(std::shared_ptr<Player> player, std::optional<BoundingBox> lastBound) {
	if (!player) return;

	Vector3 playerPos = player->GetPosition();
	BoundingBox currentViewBounds(Vector3(playerPos.x - 9, 0, playerPos.z - 5), Vector3(playerPos.x + 9, 0, playerPos.z + 5));

	Protocol::S_SPAWN spawnPacket;
	Protocol::S_DESPAWN despawnPacket;

	// 새로운 시야 내 객체를 저장할 임시 컨테이너
	std::unordered_set<std::shared_ptr<Object>> currentVisibleObjects;

	auto ProcessVisibility = [&](auto* quadTree) {
		if (!quadTree) return;

		auto visible = quadTree->QueryRange(currentViewBounds);
		for (auto& object : visible) {
			currentVisibleObjects.insert(object);

			// 이전에 보이지 않았던 객체(SPAWN)
			if (player->visibleObjects.find(object) == player->visibleObjects.end()) {
				if (auto creature = std::dynamic_pointer_cast<Creature>(object)) {
					if (creature->IsDead()) continue;
				}

				Protocol::ObjectInfo* info = spawnPacket.add_objects();
				info->CopyFrom(object->GetObjectInfo());
			}
		}
		};

	// 플레이어와 몬스터 쿼드트리 처리
	ProcessVisibility(QuadTreeManager::GetInstance().GetPlayerQuadTree(_roomId));
	ProcessVisibility(QuadTreeManager::GetInstance().GetMonsterQuadTree(_roomId));

	// 이전 시야에 있었지만 현재 시야에 없는 객체(DESPAWN)
	for (auto& object : player->visibleObjects) {
		if (currentVisibleObjects.find(object) == currentVisibleObjects.end()) {
			despawnPacket.add_objectids(object->GetId());
		}
	}

	// 현재 시야를 visibleObjects로 갱신
	player->visibleObjects = std::move(currentVisibleObjects);

	// SPAWN 패킷 전송
	if (spawnPacket.objects_size() > 0) {
		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(spawnPacket);
		}
	}

	// DESPAWN 패킷 전송
	if (despawnPacket.objectids_size() > 0) {
		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(despawnPacket);
		}
	}
}


void Room::Enter(std::shared_ptr<Object> gameObject) {

	if (!gameObject) return;

	//새로들어온게 플레이어일경우
	if (auto player = std::dynamic_pointer_cast<Player>(gameObject))
	{
		if (player->isDummy == false)
			player->SetPosition(Vector3(0, 0, 0));

		_playerCharacters[gameObject->GetId()] = player;
		player->SetRoom(static_pointer_cast<Room>(shared_from_this()));

		auto* currentPlayerQuadTree = QuadTreeManager::GetInstance().GetPlayerQuadTree(_roomId);

		currentPlayerQuadTree->Insert(player);

		if (_mapDataNumber == 0)	LOG_EVENT(Color::BLUE, L"[Room] Player entered. ID: %d, Map : Field\n", player->GetId());
		else	LOG_EVENT(Color::BLUE, L"[Room] Player entered. ID: %d, Map : Dungeon\n", player->GetId());

		//게임 접속 패킷
		Protocol::S_ENTER_ROOM enterPacket;

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(player->GetObjectInfo());

		enterPacket.set_allocated_player(playerInfo);
		enterPacket.set_mapid(GetMapId());

		//전송부
		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(enterPacket);
		}

		// 시야 내 객체 전송
		SendVisibleObjects(player);
	}
	//몬스터일 경우
	else if (auto monster = std::dynamic_pointer_cast<Monster>(gameObject)) {

		_monsters[gameObject->GetId()] = monster;
		monster->SetRoom(static_pointer_cast<Room>(shared_from_this()));
	}

	//들어온 ID제외 스폰 패킷 발송
	Protocol::S_SPAWN spawnPacket;
	Protocol::ObjectInfo* objectInfo = spawnPacket.add_objects();
	objectInfo->CopyFrom(gameObject->GetObjectInfo());

	Broadcast(spawnPacket, std::dynamic_pointer_cast<Creature>(gameObject), gameObject->GetId());
}

void Room::Leave(int32 objectId) {
	Protocol::S_DESPAWN despawnPacket;
	despawnPacket.add_objectids(objectId);

	auto it = _playerCharacters.find(objectId);
	if (it != _playerCharacters.end())
	{
		auto player = it->second;

		Broadcast(despawnPacket, player);

		// 시야 범위 계산
		Vector3 playerPos = player->GetPosition();
		BoundingBox viewBounds(Vector3(playerPos.x - 15, 0, playerPos.z - 10),
			Vector3(playerPos.x + 15, 0, playerPos.z + 10));

		auto* currentPlayerQuadTree = QuadTreeManager::GetInstance().GetPlayerQuadTree(GetRoomId());
		auto* currentMonsterQuadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(GetRoomId());

		std::vector<std::shared_ptr<Player>> visiblePlayers;
		if (currentPlayerQuadTree)
			visiblePlayers = currentPlayerQuadTree->QueryRange(viewBounds);

		std::vector<std::shared_ptr<Monster>> visibleMonsters;
		if (currentMonsterQuadTree)
			visibleMonsters = currentMonsterQuadTree->QueryRange(viewBounds);

		Protocol::S_LEAVE_ROOM leavePacket;
		leavePacket.set_objectid(objectId);

		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(leavePacket);
		}

		Protocol::S_DESPAWN despawnOtherPacket;

		for (const auto& visiblePlayer : visiblePlayers)
		{
			if (visiblePlayer->GetId() != objectId)
			{
				despawnOtherPacket.add_objectids(visiblePlayer->GetId());
			}
		}
		for (const auto& visibleMonster : visibleMonsters)
		{
			if (visibleMonster->GetId() != objectId)
			{
				despawnOtherPacket.add_objectids(visibleMonster->GetId());
			}
		}

		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(despawnOtherPacket);
		}


		// 플레이어 제거
		_playerCharacters.erase(objectId);
		auto* playerQuadTree = QuadTreeManager::GetInstance().GetPlayerQuadTree(GetRoomId());
		if (playerQuadTree)
			playerQuadTree->Remove(player);

		LOG_EVENT(Color::BLUE, L"[Room] Player left. ID: %d\n", objectId);
	}
	else if (_monsters.contains(objectId))
	{


		Broadcast(despawnPacket, _monsters[objectId]);
		_monsters.erase(objectId);

		LOG_EVENT(Color::GREEN, L"[Room] Monster left. ID: %d\n", objectId);
	}

}

void Room::SyncPos(std::shared_ptr<Player> player, Protocol::PositionInfo posInfo)
{
	if (!player) return;

	player->SetPositionInfo({ posInfo });

	Vector3 newPosition = player->GetPosition();

	// 새로운 MBR 범위 확인
	BoundingBox newBounds(Vector3(newPosition.x - 9, 0, newPosition.z - 5),
		Vector3(newPosition.x + 9, 0, newPosition.z + 5));
	BoundingBox lastBound = player->GetLastBounds();
	if (player->SetLastBounds(newBounds)) {

		auto* playerQuadTree = QuadTreeManager::GetInstance().GetPlayerQuadTree(GetRoomId());
		if (playerQuadTree)
		{
			playerQuadTree->Remove(player);
			playerQuadTree->Insert(player);
		}

		SendVisibleObjects(player, lastBound);
	}

	return;
}

void Room::HandleMove(std::shared_ptr<Player> player, Protocol::PositionInfo posInfo, uint64_t timestamp)
{
	if (!player) return;

	Protocol::S_MOVE movePacket;
	{
		movePacket.set_objectid(player->GetId());
		movePacket.set_timestamp(timestamp);
		Protocol::PositionInfo* info = movePacket.mutable_posinfo();
		info->CopyFrom(posInfo);
	}

	Broadcast(movePacket, player);


}



void Room::HandleDamage(std::shared_ptr<Player> player, const BoundingBox& damageRange, const std::function<bool(std::shared_ptr<Monster>)>& condition, int32 damage)
{
	std::vector<std::shared_ptr<Monster>> viewMonsters;

	auto* monsterQuadTree = QuadTreeManager::GetInstance().GetMonsterQuadTree(GetRoomId());
	if (monsterQuadTree)
	{
		viewMonsters = monsterQuadTree->QueryRange(damageRange);
	}

	for (auto& monster : viewMonsters) {
		if (!monster || monster->IsDead()) {
			continue;
		}

		// 몬스터가 조건에 맞는지 확인
		if (condition(monster)) {
			monster->OnDamaged(damage, player);

		}
	}

}

void Room::HandleAttack(std::shared_ptr<Player> player, float dirX, float dirZ)
{
	LOG_EVENT(Color::WHITE, L"[Room] Player attack. Player ID: %d, DirX: %f, DirZ: %f\n", player->GetId(), dirX, dirZ);

	Vector3 attackDir = { dirX, 0, dirZ };
	attackDir.Normalize();

	Vector3 playerPos = player->GetPosition();
	BoundingBox viewBounds(Vector3(playerPos.x - 10, 0, playerPos.z - 7),
		Vector3(playerPos.x + 10, 0, playerPos.z + 7));

	float attackRange = 2.5f;
	float attackAngle = 60.0f;

	Protocol::S_ATTACK attackPacket;
	attackPacket.set_attackerid(player->GetId());
	attackPacket.set_dirx(dirX);
	attackPacket.set_dirz(dirZ);

	HandleDamage(player, viewBounds,
		[&](std::shared_ptr<Monster> monster) {
			Vector3 toMonster = monster->GetPosition() - player->GetPosition();
			toMonster.Normalize();
			return Vector3::Distance(player->GetPosition(), monster->GetPosition()) <= attackRange &&
				Vector3::Angle(attackDir, toMonster) <= attackAngle;
		},
		player->GetStat().attack()
	);


	Broadcast(attackPacket, player);
}

void Room::HandleUseItem(std::shared_ptr<Player> player, int32 itemDbId) {

	auto item = player->GetInventory().Get(itemDbId);
	if (!item) return;

	LOG_EVENT(Color::WHITE, L"[Room] Player used item. Player ID: %d, Item DB ID: %d\n", player->GetId(), itemDbId);

	if (item->GetItemType() == Protocol::ItemType::ITEM_TYPE_CONSUMABLE) {
		item->ApplyEffects(player);
		if (item->GetCount() == 1)
		{
			player->GetInventory().Remove(itemDbId);

			DB_ItemService::RemoveItem(item->GetItemDbId());
		}
		else
		{
			item->SetCount(item->GetCount() - 1);

			DB_ItemService::UpdateItem(item->GetItemDbId(), item->GetCount(), item->IsEquipped());
		}
	}
	else if (item->IsEquipable()) {
		bool isEquip = false;

		if (!(item->IsEquipped()))
			isEquip = true;

		UnequipExistingItem(player, item->GetItemType());

		if (isEquip)
		{
			item->SetEquipped(true);
			item->ApplyEffects(player);
			player->GetInventory().Add(item);

			DB_ItemService::UpdateItem(item->GetItemDbId(), item->GetCount(), item->IsEquipped());
		}
	}

	Protocol::S_CHANGE_ITEM_INFO packet;
	{
		Protocol::ItemInfo itemInfo = item->GetItemInfo();

		Protocol::ItemInfo* packetItemInfo = packet.add_items();
		*packetItemInfo = itemInfo;
	}

	if (auto session = player->GetSession().lock()) {
		SEND_PACKET(packet);
	}
}

void Room::HandleRemoveItem(std::shared_ptr<Player> player, int32 itemDbId) {
	player->GetInventory().Remove(itemDbId);

	DB_ItemService::RemoveItem(itemDbId);
}


void Room::UnequipExistingItem(std::shared_ptr<Player> player, Protocol::ItemType itemType) {
	auto equippedItem = player->GetInventory().UnequipItem(itemType);
	auto inven = player->GetInventory();
	if (equippedItem)
	{
		equippedItem->SetEquipped(false);
		equippedItem->RemoveEffects(player);
		inven.Change(equippedItem);
		DB_ItemService::UpdateItem(equippedItem->GetItemDbId(), equippedItem->GetCount(), equippedItem->IsEquipped());

		Protocol::S_CHANGE_ITEM_INFO packet;
		{
			Protocol::ItemInfo itemInfo = equippedItem->GetItemInfo();

			Protocol::ItemInfo* packetItemInfo = packet.add_items();
			*packetItemInfo = itemInfo;
		}

		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(packet);
		}
	}

}

std::queue<Vector3> Room::GetPath(const Vector3& start, const Vector3& goal) {
	std::queue<Vector3> path;

	if (!hasMapData) return path; // 맵 데이터가 없으면 빈 경로 반환

	// A* 경로 계산
	int startX = static_cast<int>(start.x);
	int startY = static_cast<int>(start.z);
	int goalX = static_cast<int>(goal.x);
	int goalY = static_cast<int>(goal.z);

	auto result = _astar->FindPath(startX, startY, goalX, goalY);

	for (const auto& [x, y] : result) {
		path.push({ static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	return path;
}

void Room::HandleSkill(std::shared_ptr<Player> player, int32 skillId, Protocol::PositionInfo targetPos)
{
	if (!player) return;

	auto it = DataManager::GetSkillDict().find(skillId);
	if (it == DataManager::GetSkillDict().end()) {
		std::cerr << "Invalid skillId: " << skillId << std::endl;
		return;
	}

	const auto& skillData = *it->second;

	if (player->GetMana() < skillData.cost || !player->CanUseSkill(skillId)) {
		Protocol::S_SKILL falseSkillPacket;
		falseSkillPacket.set_success(false);

		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(falseSkillPacket);
		}
		return;
	}

	player->ReduceMana(skillData.cost);
	player->StartCooldown(skillId, skillData.cooldown);

	Protocol::S_SKILL skillPacket;
	skillPacket.set_skilltemplateid(skillId);

	skillPacket.set_attackerid(player->GetId());
	skillPacket.set_success(true);

	if (skillData.type == "area") {

		auto areaData = static_cast<const Area&>(skillData);

		BoundingBox aroundBounds(Vector3(targetPos.posx() - (areaData.range * 2), 0, targetPos.posz() - (areaData.range * 2)),
			Vector3(targetPos.posx() + (areaData.range * 2), 0, targetPos.posz() + (areaData.range * 2)));

		HandleDamage(player, aroundBounds,
			[&](std::shared_ptr<Monster> monster) {
				return Vector3::Distance({ targetPos.posx(), 0.0f, targetPos.posz() }, monster->GetPosition()) <= areaData.range;
			},
			areaData.damage);

		skillPacket.mutable_posinfo()->CopyFrom(targetPos);
	}
	else if (skillData.type == "directional") {
		auto directionalData = static_cast<const Directional&>(skillData);

		Vector3 playerPos = player->GetPosition();
		BoundingBox aroundBounds(Vector3(playerPos.x - (directionalData.range * 2), 0, playerPos.z - (directionalData.range * 2)),
			Vector3(playerPos.x + (directionalData.range * 2), 0, playerPos.z + (directionalData.range * 2)));

		Vector3 start = player->GetPosition();
		Vector3 direction = { targetPos.posx() - start.x, 0.0f, targetPos.posz() - start.z };
		direction.Normalize();

		HandleDamage(player, aroundBounds,
			[&](std::shared_ptr<Monster> monster) {
				Vector3 toMonster = monster->GetPosition() - start;
				return Vector3::Distance(start, monster->GetPosition()) <= directionalData.range &&
					Vector3::Angle(direction, toMonster.Normalize()) <= directionalData.angle / 2;
			},
			directionalData.damage);

		skillPacket.mutable_posinfo()->CopyFrom(player->GetPositionInfo());
	}

	Broadcast(skillPacket, player);
}