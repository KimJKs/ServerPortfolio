#pragma once
#include "Vector3.h"

class Room;

class Object : public std::enable_shared_from_this<Object> {
public:
	Object();
	virtual ~Object();

	// Basic Info
	virtual void Update();

	// Getters and Setters
	int32 GetId() const { return _objectId; }
	void SetId(int32 id) { _objectId = id; _objectInfo.set_objectid(id); }

	Vector3 GetPosition() const {
		const Protocol::PositionInfo& posInfo = _objectInfo.posinfo();
		return Vector3(posInfo.posx(), 0, posInfo.posz());
	}
	void SetPosition(const Vector3& pos) {
		Protocol::PositionInfo* posInfo = _objectInfo.mutable_posinfo();
		posInfo->set_posx(pos.x);
		posInfo->set_posz(pos.z);
	}

	shared_ptr<Room> GetRoom() const { return _room.lock(); }
	void SetRoom(const shared_ptr<Room>& newRoom) { _room = newRoom; }

	Protocol::ObjectType GetObjectType() const { return _objectType; }
	void SetObjectType(Protocol::ObjectType type) {
		_objectType = type;
		
	}

	Protocol::ObjectInfo& GetObjectInfo() { return _objectInfo; }
	void SetObjectInfo(const Protocol::ObjectInfo& info) { _objectInfo = info; }

	const Protocol::PositionInfo& GetPositionInfo() const { return _objectInfo.posinfo(); }
	void SetPositionInfo(const Protocol::PositionInfo& info) {
		Protocol::PositionInfo* posInfo = this->GetObjectInfo().mutable_posinfo();
		posInfo->CopyFrom(info);
	}


	void SetObjectName(string name) { _objectInfo.set_name(name); }

protected:
	int32 _objectId;
	Protocol::ObjectType _objectType;
	Protocol::ObjectInfo _objectInfo;
	std::weak_ptr<Room> _room;
};
