#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <regex>
#include <condition_variable>
#include <functional>
#include <latch>
using namespace std;

class ThreadPool {
public:
    ThreadPool(bool verbose, const std::string& expr);
    ~ThreadPool();

    void start();
    void stop();
    void enqueueTask(const filesystem::directory_entry& task);

    vector<string> report;
    int matches;

private:
    void workerThread();

    vector<thread> workers;
    queue<filesystem::directory_entry> tasks;
    mutex queueMutex;
    mutex consoleMutex;
    condition_variable wakeCond;
    bool stopFlag;
    bool verbose;
    int numThreads;
    latch setupDone{ numThreads + 1 };
    
    regex expr;
};

