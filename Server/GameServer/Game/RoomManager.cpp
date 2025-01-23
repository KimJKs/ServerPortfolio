#include "pch.h"
#include "RoomManager.h"
#include <future>

shared_ptr<RoomManager> GRoomManager = make_shared<RoomManager>();

void RoomManager::Update() {
	for (auto& [roomId, room] : _rooms) {
		if (room && room->GetPlayers().size() > 0) {
			room->Update();
		}
	}

	DoTimer(80, &RoomManager::Update);
}


std::vector<Vector3> _dungeonspawnZones = {
	{5.0f, 0.0f, -3.0f},
	{0.0f, 0.0f, 5.0f},
	{-3.0f, 0.0f, -5.0f}
};

std::shared_ptr<Room> RoomManager::Add(int mapId) {
	auto room = std::make_shared<Room>();
	room->SetMapDataNumber(mapId);
	room->InitRoom(); // Room 초기화 작업
	room->SetRoomId(_nextRoomId);
	_rooms[_nextRoomId] = room;
	_nextRoomId++;


	// 스폰 존 초기화
	if (mapId == 0) {
		room->InitMonsterInField();
		LOG_EVENT(Color::GREEN, L"[RoomManager] Created Field Room.\n");
	}
	else {
		room->InitMonsterSpawn(_dungeonspawnZones);
		LOG_EVENT(Color::YELLOW, L"[RoomManager] Created Dungeon Room.\n");
	}


	return room;
}

bool RoomManager::Remove(int roomId) {
	return _rooms.erase(roomId) > 0;
}

std::shared_ptr<Room> RoomManager::Find(int roomId) {
	auto it = _rooms.find(roomId);
	if (it != _rooms.end()) {
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Room> RoomManager::FindByMapId(int mapId)
{
	for (auto& [id, room] : _rooms)
	{
		if (room->GetMapId() == mapId)
			return room;
	}
	return nullptr;
}

std::shared_ptr<Player> RoomManager::FindPlayerByUserDbId(int32 userDbId) 
{
	for (auto& [roomId, room] : _rooms) {
		auto players = room->GetPlayers();
		for (const auto& [id, player] : players) {
			if (player->GetSession().lock()->GetUserStatus()->GetUserId() == userDbId) 
			{
				return player;
			}
		}
	}
	return nullptr; 
}

std::shared_ptr<Player> RoomManager::FindPlayerByUserName(wstring userName)
{
	for (auto& [roomId, room] : _rooms) {
		auto players = room->GetPlayers();
		for (const auto& [id, player] : players) {
			if (player->GetSession().lock()->GetUserStatus()->GetUsername() == userName)
			{
				
				return player;
			}
		}
	}
	return nullptr;
}