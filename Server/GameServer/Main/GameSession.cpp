#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "Room.h"

// 전역 변수 초기화
std::atomic<int64_t> recvPacketCount = 0;
std::atomic<int64_t> sendPacketCount = 0;
std::atomic<int> sessionCount = 0;
std::chrono::steady_clock::time_point lastMeasureTime = std::chrono::steady_clock::now();



void GameSession::OnConnected()
{
	sessionCount.fetch_add(1); // 활성 세션 수 증가
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	sessionCount.fetch_sub(1); // 활성 세션 수 감소
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	if (player)
	{
		if (auto room = _room.lock())
		{
			room->DoAsync(&Room::Leave, player->GetId());
		}
	}

	player = nullptr;
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);

	recvPacketCount++; // 수신 카운트 증가
}

void GameSession::OnSend(int32 len)
{
	sendPacketCount++; // 송신 카운트 증가
}

void GameSession::Send(SendBufferRef sendBuffer)
{
	std::lock_guard<std::mutex> lock(_sendMutex);
	_sendQueue.push(sendBuffer);
	_reservedBytes += sendBuffer->WriteSize();
}

void GameSession::FlushSend()
{
	std::vector<SendBufferRef> sendList;
	int64_t totalBytes = 0;

	{
		std::lock_guard<std::mutex> lock(_sendMutex);

		auto now = std::chrono::steady_clock::now();
		if (_sendQueue.empty() ||
			(_reservedBytes < 10240 && std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastSendTime).count() < 100))
		{
			return;
		}

		while (!_sendQueue.empty())
		{
			auto packet = _sendQueue.front();
			sendList.push_back(packet);
			totalBytes += packet->WriteSize();
			_sendQueue.pop();
		}

		_reservedBytes = 0;
		_lastSendTime = now;
	}

	Session::Send(sendList);

}
