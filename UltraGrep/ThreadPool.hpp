#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
using namespace std;

class ThreadPoolFacade
{
	ThreadPoolFacade(size_t numThreads);
	~ThreadPoolFacade();

	void start();
	void stop();
	void enqueueTask(function<void()> task);
private:
	class ThreadPool { 
	public: 
		ThreadPool(size_t numThreads); 
		~ThreadPool();

		void enqueueTask(function<void()> task);
		void start();
		void stop();
	private:
		void workerThread();

		vector<thread> workers;
		queue<function<void()>> tasks;
		mutex queueMutex;
		condition_variable condition;
		bool stopFlag; 
	}; 
	ThreadPool pool;
};

