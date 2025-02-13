#pragma once

class ServerMonitor : public JobQueue
{
public:

	void MeasureTPS();

};

// TPS 관련 전역 변수
extern std::atomic<int64_t> recvPacketCount;
extern std::atomic<int64_t> sendPacketCount;
extern std::chrono::steady_clock::time_point lastMeasureTime;

extern shared_ptr<ServerMonitor> GServerMonitor;