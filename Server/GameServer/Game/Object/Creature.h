#pragma once
#include "Object.h"
#include "AStar.h"
#include "BoundingBox.h" // BoundingBox�� ����

class Creature : public Object {
public:
	Creature();
	virtual ~Creature();

	virtual void OnDamaged(int32 damage, std::shared_ptr<Object> attacker);
	virtual void OnDead(std::shared_ptr<Object> attacker);

	bool IsDead() const;



	const Protocol::StatInfo& GetStat() const { return _objectInfo.statinfo(); }

	void SetStat(const Protocol::StatInfo& value) {
		Protocol::StatInfo* statInfo = _objectInfo.mutable_statinfo();
		statInfo->CopyFrom(value);
	}

	const Protocol::CreatureState& GetCreatureState() const { return _objectInfo.posinfo().state(); }

	void SetCreatureState(const Protocol::CreatureState& value) {
		Protocol::PositionInfo* posInfo = _objectInfo.mutable_posinfo();
		posInfo->set_state(value);
	}

	void SetPath(const AStar::Path& path) {
		_path = path;
		_currentPathIndex = 0; // �� ��ΰ� �����Ǹ� ó������ ����
	}

	std::pair<int, int> GetNowPath()
	{
		return _path[_currentPathIndex];
	}

	std::pair<int, int> GetNextPath()
	{
		if (HasPath())
		{
			_currentPathIndex++;
			return _path[_currentPathIndex];
		}

		return { _objectInfo.posinfo().posx(), _objectInfo.posinfo().posz() };
	}
	bool HasPath() const {
		return _currentPathIndex < _path.size() - 1;
	}

	int32 GetMana() const { return _objectInfo.statinfo().mp(); }

	void ReduceMana(int32 amount);

	bool CanUseSkill(int32 skillId) const {
		auto it = cooldowns.find(skillId);
		if (it == cooldowns.end()) return true;

		int64 currentTime = GetCurrentTime();
		return currentTime >= it->second;
	}

	void StartCooldown(int32 skillId, int32 cooldown) {
		cooldowns[skillId] = GetCurrentTime() + cooldown;
	}

	const BoundingBox& GetLastBounds() const { return _lastBounds; }

	bool SetLastBounds(const BoundingBox& bounds) {
		using namespace std::chrono;

		// ���� �ð� ��������
		auto now = steady_clock::now();
		auto elapsed = duration_cast<milliseconds>(now - _lastBoundsUpdateTime).count();

		// ���� ������ ���ο� ������ �����ϸ� false ��ȯ
		if (_lastBounds == bounds)
			return false;

		// ���� �ð��� ������ �ʾҴٸ� false ��ȯ
		if (elapsed < _boundsUpdateIntervalMs)
			return false;

		// ������Ʈ ����
		_lastBounds = bounds;
		_lastBoundsUpdateTime = now; // ������ ������Ʈ �ð� ����
		return true;
	}


private:
	AStar::Path _path;
	size_t _currentPathIndex = 0; // ���� ����� �ε���
	std::unordered_map<int32, int64> cooldowns;

protected:
	BoundingBox _lastBounds;             
	std::chrono::steady_clock::time_point _lastBoundsUpdateTime; // ������ ������Ʈ �ð�
	int32 _boundsUpdateIntervalMs = 500; // �⺻ ������Ʈ ���� (�и���)
};
