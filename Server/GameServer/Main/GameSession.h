#pragma once
#include "Session.h"
#include <memory>
#include "Player.h"


class GameSession : public PacketSession
{
public:
	~GameSession()
	{
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

	void				Send(SendBufferRef sendBuffer);
	void FlushSend();

	const shared_ptr<UserStatus>& GetUserStatus() { return _userStatus; }
	void SetUserStatus(const shared_ptr<UserStatus>& value) { _userStatus = value; }

public:

	shared_ptr<Player> player;
	weak_ptr<class Room> _room;
	shared_ptr<UserStatus> _userStatus;

private:
	std::queue<SendBufferRef> _sendQueue; // 송신 패킷 큐
	std::mutex _sendMutex;               // 멀티스레드 보호용
	int32 _reservedBytes = 0; // 현재 예약된 바이트 수
	std::chrono::steady_clock::time_point _lastSendTime = std::chrono::steady_clock::now();

};
// TPS 관련 전역 변수
extern std::atomic<int64_t> recvPacketCount;
extern std::atomic<int64_t> sendPacketCount;
extern std::atomic<int> sessionCount;
extern std::chrono::steady_clock::time_point lastMeasureTime;
