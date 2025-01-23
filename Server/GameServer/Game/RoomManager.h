#pragma once

#include "Room.h"

class RoomManager : public JobQueue {
public:
	static RoomManager& GetInstance() {
		static RoomManager instance;
		return instance;
	}

	void Update();

	std::shared_ptr<Room> Add(int mapId);
	bool Remove(int roomId);
	std::shared_ptr<Room> Find(int roomId);
	std::shared_ptr<Room> FindByMapId(int mapId);
	std::shared_ptr<Player> FindPlayerByUserDbId(int32 userDbId);

	std::shared_ptr<Player> FindPlayerByUserName(wstring userName);

private:
	std::unordered_map<int, std::shared_ptr<Room>> _rooms;
	int32 _nextRoomId = 1;
};

extern shared_ptr<RoomManager> GRoomManager;