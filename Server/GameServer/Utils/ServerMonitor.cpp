#include "pch.h"
#include "ServerMonitor.h"
#include "GameSessionManager.h"

shared_ptr<ServerMonitor> GServerMonitor = make_shared<ServerMonitor>();

void ServerMonitor::MeasureTPS()
{
	auto now = std::chrono::steady_clock::now();
	double duration = std::chrono::duration<double>(now - lastMeasureTime).count();

	if (duration == 0.0)
	{
		DoTimer(1000, &ServerMonitor::MeasureTPS);
		return;
	}

	// TPS 계산
	int64 recvTPS = static_cast<int64>(recvPacketCount.exchange(0) / duration);
	int64 sendTPS = static_cast<int64>(sendPacketCount.exchange(0) / duration);

	lastMeasureTime = now;

	// 로그 출력
	std::cout << "[TPS] Recv: " << recvTPS << " / Send: " << sendTPS
		<< " / Active Sessions: " << GSessionManager.GetSessionCount() << std::endl;

	// 1초 후 다시 실행하도록 `JobTimer`에 예약
	DoTimer(1000, &ServerMonitor::MeasureTPS);
}


// 전역 변수 초기화
std::atomic<int64> recvPacketCount = 0;
std::atomic<int64> sendPacketCount = 0;
std::chrono::steady_clock::time_point lastMeasureTime = std::chrono::steady_clock::now();
