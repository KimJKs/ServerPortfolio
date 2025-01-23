#pragma once

class ObjectManager {
public:
	static ObjectManager& Instance()
	{
		static ObjectManager instance; // 단일 인스턴스
		return instance;
	}

	template <typename T>
	std::shared_ptr<T> Add() {
		bool isObejct = std::is_base_of<Object, T>::value;
		ASSERT_CRASH(isObejct);
		auto gameObject = std::make_shared<T>();

		gameObject->SetId(GenerateId(gameObject->GetObjectType()));

		if constexpr (std::is_same_v<T, Player>)
		{
			_players[gameObject->GetId()] = std::static_pointer_cast<Player>(gameObject);
		}

		return gameObject;
	}

	bool Remove(int objectId);
	std::shared_ptr<Player> FindPlayer(int objectId);
	static Protocol::ObjectType GetObjectTypeById(int id);

private:
	ObjectManager() = default;
	~ObjectManager() = default;
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;

	int GenerateId(Protocol::ObjectType type);

	std::mutex _mutex;
	std::unordered_map<int, std::shared_ptr<Player>> _players;
	int _counter = 0;
};
