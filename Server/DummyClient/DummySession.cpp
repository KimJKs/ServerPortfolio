#include "pch.h"
#include "DummySession.h"
#include "ClientPacketHandler.h"
#include "JobQueue.h"
#include "Vector3.h"
#include <algorithm>
#include <random>

// ���� ���� ����
static int s_dummyIdCounter = 1;

DummySession::DummySession()
{
	_playerInfo = new Protocol::ObjectInfo();

	// ���� ID�� �̸� ����
	std::string playerName = "Dummy" + std::to_string(s_dummyIdCounter++);
	_playerInfo->set_name(playerName);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(-90, 90);
	std::uniform_real_distribution<float> distZ(-90, 90);

	float x = distX(gen);
	float z = distZ(gen);

	Protocol::PositionInfo* posinfo = _playerInfo->mutable_posinfo();
	posinfo->set_posx(x);
	posinfo->set_posz(z);

	spawnPosX = x;
	spawnPosZ = z;
}

DummySession::~DummySession()
{
}

void DummySession::OnConnected()
{
	Protocol::C_ENTER_GAME enterGamePkt;
	enterGamePkt.set_isdummy(true);

	// _playerInfo �����͸� �����Ͽ� ����
	Protocol::ObjectInfo* info = enterGamePkt.mutable_objectinfo();
	info->CopyFrom(*_playerInfo);

	shared_ptr<SendBuffer> sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	Send(sendBuffer);

	UpdateTick();
}

void DummySession::OnDisconnected()
{
}

void DummySession::OnRecvPacket(BYTE* buffer, int32 len)
{
}

void DummySession::OnSend(int32 len)
{
}

void DummySession::UpdateTick()
{
	if (_playerInfo == nullptr)
		return;

	// ���� ���� ����
	Vector3 currentPos = { _playerInfo->posinfo().posx(), 0.0f, _playerInfo->posinfo().posz() };
	Vector3 randomDirection = GetRandomDirection();

	// �̵� ���� ����: ���� ��ġ���� 3x3 ���� ��
	Vector3 newPos = currentPos + randomDirection * 2.0f;
	newPos.x = std::clamp(newPos.x, spawnPosX - 3.0f, spawnPosX + 3.0f);
	newPos.z = std::clamp(newPos.z, spawnPosZ - 3.0f, spawnPosZ + 3.0f);

	// �̵� ��Ŷ ����
	Protocol::C_MOVE movePkt;
	{
		Protocol::PositionInfo* info = movePkt.mutable_posinfo();
		info->set_posx(newPos.x);
		info->set_posz(newPos.z);
		info->set_state(Protocol::CreatureState::MOVING);
	}

	shared_ptr<SendBuffer> sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
	Send(sendBuffer);

	// ���ο� ��ġ�� _playerInfo�� ����
	Protocol::PositionInfo* info = _playerInfo->mutable_posinfo();
	info->set_posx(newPos.x);
	info->set_posz(newPos.z);

	// ���� �̵� ����
	std::random_device rd; // �õ� ������
	std::mt19937 gen(rd()); // ���� ���� ����
	std::uniform_int_distribution<int> dist(500, 1000); // 500ms ~ 1000ms ����

	int randomDelay = dist(gen); // ������ ������ �ð� ����

	shared_ptr<Job> job = make_shared<Job>(GetDummyClientSessionRef(), &DummySession::UpdateTick);
	GJobTimer->Reserve(randomDelay, GJobQueue, job);
}

Vector3 DummySession::GetRandomDirection()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	Vector3 direction(dist(gen), 0.0f, dist(gen));
	direction.Normalize();
	return direction;
}
