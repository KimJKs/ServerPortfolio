#pragma once

class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager : public JobQueue
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

	int32 GetSessionCount() { return _sessions.size(); }
	
	vector<GameSessionRef> GetSessions();

	void FlushSessionPacket();

private:
	USE_LOCK;
	set<GameSessionRef> _sessions;
};

extern shared_ptr<GameSessionManager> GSessionManager;
