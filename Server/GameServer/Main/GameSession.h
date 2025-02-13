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

	bool HasSendPacket() { return !_sendQueue.empty(); }

public:

	shared_ptr<Player> player;
	weak_ptr<class Room> _room;
	shared_ptr<UserStatus> _userStatus;

private:
	std::queue<SendBufferRef> _sendQueue; // �۽� ��Ŷ ť
	std::mutex _sendMutex;               // ��Ƽ������ ��ȣ��
	int32 _reservedBytes = 0; // ���� ����� ����Ʈ ��
	std::chrono::steady_clock::time_point _lastSendTime = std::chrono::steady_clock::now();

};

