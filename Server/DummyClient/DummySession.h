#pragma once
class Vector3;

class DummySession	: public PacketSession
	{
	public:
		DummySession();
		virtual ~DummySession();

		virtual void OnConnected() override;
		virtual void OnDisconnected() override;
		virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
		virtual void OnSend(int32 len) override;

		void SetPlayerInfo(const Protocol::ObjectInfo& playerInfo) { _playerInfo->MergeFrom(playerInfo); }

	private:
		shared_ptr<DummySession> GetDummyClientSessionRef() { return static_pointer_cast<DummySession>(GetPacketSessionRef()); }
		void UpdateTick();
		void SendChatPacket();
		Vector3 GetRandomDirection();

	private:
		Protocol::ObjectInfo* _playerInfo;
		float spawnPosX = 0;
		float spawnPosZ = 0;
};

