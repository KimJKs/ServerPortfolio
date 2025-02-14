#pragma once
class DBWorker
{
public:
    DBWorker();
    ~DBWorker();

    void AddJob(std::function<void()> job);
    void WorkerThread();

private:

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobQueue;
    std::mutex jobMutex;
    std::condition_variable jobCond;
    bool running = true;
};

extern std::shared_ptr<DBWorker> GDBWorker;
