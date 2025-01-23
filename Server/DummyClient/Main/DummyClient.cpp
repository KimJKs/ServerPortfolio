#include "pch.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "ThreadManager.h"
#include "DummySession.h"
#include <iostream>

enum
{
    WORKER_TICK = 64
};

void DoWorkerJob(const shared_ptr<ClientService>& service)
{
    while (true)
    {
        LEndTickCount = ::GetTickCount64() + WORKER_TICK;

        service->GetIocpCore()->Dispatch(10);

        ThreadManager::DistributeReservedJobs();

        // 글로벌 큐에 쌓인 일감을 처리
        ThreadManager::DoGlobalQueueWork();
    }
}

int main()
{
    ClientPacketHandler::Init();

    int clientCount = 0;
    std::cout << "Enter the number of dummy clients to create: ";
    std::cin >> clientCount;

    if (clientCount <= 0)
    {
        std::cerr << "Invalid client count. Exiting." << std::endl;
        return -1;
    }

    ClientServiceRef service = make_shared<ClientService>(
        NetAddress(L"127.0.0.1", 7777),
        make_shared<IocpCore>(),
        [=]() ->shared_ptr<DummySession> { return make_shared<DummySession>(); },
        clientCount);

    ASSERT_CRASH(service->Start());

    GThreadManager->Launch([&service]()
        {
            DoWorkerJob(service);
        });

    DoWorkerJob(service);

    GThreadManager->Join();

    return 0;
}
