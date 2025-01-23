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

void MeasureTPS()
{
	while (true)
	{
		using namespace std::chrono;
		std::this_thread::sleep_for(1s);

		auto now = steady_clock::now();
		auto duration = duration_cast<seconds>(now - lastMeasureTime).count();
		if (duration == 0) continue;

		// TPS 계산
		int64_t recvTPS = recvPacketCount.exchange(0) / duration;
		int64_t sendTPS = sendPacketCount.exchange(0) / duration;

		lastMeasureTime = now;

		// 로그 출력
		std::cout << "[TPS] Recv: " << recvTPS << " / Send: " << sendTPS
			<< " / Active Sessions: " << sessionCount.load() << std::endl;
	}
}

void NetworkTask()
{
	while (true)
	{
		auto sessions = GSessionManager.GetSessions();
		for (const auto& session : sessions)
		{
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

	std::thread tpsThread(MeasureTPS);
	tpsThread.detach();

	std::thread networkThread(NetworkTask);
	networkThread.detach();

	//기본 필드 추가
	GRoomManager->DoAsync(&RoomManager::Add,0);
	//업데이트 작동
	GRoomManager->DoAsync(&RoomManager::Update);

	
	GDBManager->Init();
	
	GThreadManager->Join();
}