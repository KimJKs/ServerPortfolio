#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

shared_ptr<GameSessionManager> GSessionManager = make_shared<GameSessionManager>();

void GameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void GameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (GameSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}

vector<GameSessionRef> GameSessionManager::GetSessions()
{
	READ_LOCK;
	return vector<GameSessionRef>(_sessions.begin(), _sessions.end());
}

void GameSessionManager::FlushSessionPacket()
{
	auto sessions = GetSessions();
	for (const auto& session : sessions)
	{
		if (session->HasSendPacket())
			session->FlushSend();
	}

	DoTimer(10, &GameSessionManager::FlushSessionPacket);
}
