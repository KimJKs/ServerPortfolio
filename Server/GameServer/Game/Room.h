#pragma once
#include "QuadTree.h"
// Forward declaration
class Object;
class Player;
class Monster;

class Room : public JobQueue {
public:

	void Update();
	void InitRoom();

	void InitMonsterInField();
	void InitMonsterSpawn(std::vector<Vector3> spawnZones);

	template <typename PacketType>
	void Broadcast(PacketType& packet, std::shared_ptr<Creature> sender, uint64 exceptId = 0)
	{
		if (!sender)
			return;


		// 발신자의 현재 위치를 기준으로 시야 범위를 계산
		Vector3 senderPos = sender->GetPosition();
		BoundingBox viewBounds(Vector3(senderPos.x - 9, 0, senderPos.z - 5),
			Vector3(senderPos.x + 9, 0, senderPos.z + 5));

		// 시야 범위 내의 플레이어 검색
		std::vector<std::shared_ptr<Player>> visiblePlayers;
		// 플레이어 쿼드 트리에서 현재 범위 내 객체 검색

		auto* playerQuadTree = QuadTreeManager::GetInstance().GetPlayerQuadTree(_roomId);
		if (playerQuadTree)
		{
			visiblePlayers = playerQuadTree->QueryRange(viewBounds);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);

		for (const auto& player : visiblePlayers)
		{

			if (auto session = player->GetSession().lock())
			{
				if (sender->GetId() != player->GetId())
					session->Send(sendBuffer);

			}
		}

		if (auto senderPlayer = std::dynamic_pointer_cast<Player>(sender))
			if (auto senderSession = senderPlayer->GetSession().lock())
			{
				senderSession->Send(sendBuffer);

			}


	}
	void SendVisibleObjects(std::shared_ptr<Player> player, std::optional<BoundingBox> lastBound = std::nullopt);

	void Enter(std::shared_ptr<Object> object);
	void Leave(int32 objectId);

	void SyncPos(std::shared_ptr<Player> player, Protocol::PositionInfo posInfo);

	void HandleMove(std::shared_ptr<Player> player, Protocol::PositionInfo posInfo, uint64_t timestamp);
	void HandleDamage(std::shared_ptr<Player> player, const BoundingBox& damageRange, const std::function<bool(std::shared_ptr<Monster>)>& condition, int32 damage);
	void HandleAttack(std::shared_ptr<Player> player, float dirX, float dirZ);
	void HandleUseItem(std::shared_ptr<Player> player, int32 itemDbId);
	void HandleRemoveItem(std::shared_ptr<Player> player, int32 itemDbId);

	std::unordered_map<int32, std::shared_ptr<Player>> GetPlayers() { return _playerCharacters; }

	void SetMapDataNumber(int mapDataNum) { _mapDataNumber = mapDataNum; }
	int32 GetMapId() { return  _mapDataNumber; }

	void SetRoomId(int32 roomid) { _roomId = roomid; }
	int32 GetRoomId() { return _roomId; }

	std::queue<Vector3> GetPath(const Vector3& start, const Vector3& goal);

	void HandleSkill(std::shared_ptr<Player> player, int32 skillId, Protocol::PositionInfo targetPos);

	bool hasMapData = false;
private:
	void UnequipExistingItem(std::shared_ptr<Player> player, Protocol::ItemType itemType);


	std::unordered_map<int32, shared_ptr<Player>> _playerCharacters;
	std::unordered_map<int32, shared_ptr<Monster>> _monsters;

	int32 _mapDataNumber = 1;
	shared_ptr<AStar> _astar;

	int32 _roomId = -1;
};
