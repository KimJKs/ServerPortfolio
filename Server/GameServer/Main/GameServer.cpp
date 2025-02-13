#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include <tchar.h>
#include "Job.h"
#include "Room.h"
#include "Player.h"
#include "SendBuffer.h"
#include "DataManager.h"
#include "DBManager.h"
#include "RoomManager.h"
#include "ServerMonitor.h"
#include <atomic>
#include <chrono>

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

void NetworkTask()
{
	while (true)
	{
		auto sessions = GSessionManager.GetSessions();
		for (const auto& session : sessions)
		{
			if (session->HasSendPacket())
				session->FlushSend();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{

	DataManager::LoadData();

	ServerPacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); },
		1000);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 16; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	std::thread networkThread(NetworkTask);
	networkThread.detach();

	// 1초마다 TPS 측정하기추가
	GServerMonitor->DoAsync(&ServerMonitor::MeasureTPS);
	//기본 필드 추가
	GRoomManager->DoAsync(&RoomManager::Add, 0);
	//업데이트 작동
	GRoomManager->DoAsync(&RoomManager::Update);


	GDBManager->Init();

	GThreadManager->Join();
}