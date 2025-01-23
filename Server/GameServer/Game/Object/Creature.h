#pragma once
#include "Object.h"
#include "AStar.h"
#include "BoundingBox.h" // BoundingBox만 포함

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
		_currentPathIndex = 0; // 새 경로가 설정되면 처음부터 시작
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

		// 현재 시간 가져오기
		auto now = steady_clock::now();
		auto elapsed = duration_cast<milliseconds>(now - _lastBoundsUpdateTime).count();

		// 기존 범위와 새로운 범위가 동일하면 false 반환
		if (_lastBounds == bounds)
			return false;

		// 일정 시간이 지나지 않았다면 false 반환
		if (elapsed < _boundsUpdateIntervalMs)
			return false;

		// 업데이트 성공
		_lastBounds = bounds;
		_lastBoundsUpdateTime = now; // 마지막 업데이트 시간 갱신
		return true;
	}


private:
	AStar::Path _path;
	size_t _currentPathIndex = 0; // 현재 경로의 인덱스
	std::unordered_map<int32, int64> cooldowns;

protected:
	BoundingBox _lastBounds;             
	std::chrono::steady_clock::time_point _lastBoundsUpdateTime; // 마지막 업데이트 시간
	int32 _boundsUpdateIntervalMs = 500; // 기본 업데이트 간격 (밀리초)
};
