#include "pch.h"
#include "DBWorker.h"
#include <iostream>

std::shared_ptr<DBWorker> GDBWorker = make_shared<DBWorker>();


DBWorker::DBWorker()
{

}

DBWorker::~DBWorker()
{
    {
        std::lock_guard<std::mutex> lock(jobMutex);
        running = false;
    }
    jobCond.notify_all(); 

    for (auto& worker : workers)
    {
        worker.join();
    }
}

void DBWorker::AddJob(std::function<void()> job)
{
    {
        std::lock_guard<std::mutex> lock(jobMutex);
        jobQueue.push(job);
    }
    jobCond.notify_one();
}

void DBWorker::WorkerThread()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(jobMutex);
            jobCond.wait(lock, [this]() { return !jobQueue.empty() || !running; });

            if (!running && jobQueue.empty())
                return;

            job = jobQueue.front();
            jobQueue.pop();
        }

        job();
    }
}
