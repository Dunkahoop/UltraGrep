/**
     * File Name:       ThreadPool.hpp
     * Description:     Header file for ThreadPool class
     * Author:			Duncan Wade
     * Date:            November 16th, 2024
*/

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
    ThreadPool(bool verbose, const string& expr);
    ~ThreadPool();

    void start();
    void stop();
    void enqueueTask(const filesystem::directory_entry& task);

    void generateReport(chrono::duration<double, std::milli> time);
private:
    void workerThread();

    vector<jthread> workers;
    queue<filesystem::directory_entry> tasks;
    mutex queueMutex;
    mutex taskMutex;
    mutex consoleMutex;
    condition_variable wakeCond;
    bool stopFlag;
    bool verbose;
    int numThreads;
    latch setupDone{ numThreads + 1 };
    vector<string> consoleReport;
    int matches;
    int matchedFiles;
    regex expr;
};