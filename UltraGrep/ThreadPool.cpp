#include "ThreadPool.hpp"

ThreadPoolFacade::~ThreadPoolFacade()
{
	stop();
}

void ThreadPoolFacade::start()
{
	pool.start();
}

void ThreadPoolFacade::stop()
{
	pool.stop();
}

void ThreadPoolFacade::enqueueTask(function<void()> task)
{
	pool.enqueueTask(task);
}

ThreadPoolFacade::ThreadPool::ThreadPool(size_t numThreads)
{
}

ThreadPoolFacade::ThreadPool::~ThreadPool()
{
}

void ThreadPoolFacade::ThreadPool::enqueueTask(function<void()> task)
{
}

void ThreadPoolFacade::ThreadPool::start()
{
}

void ThreadPoolFacade::ThreadPool::stop()
{
}

void ThreadPoolFacade::ThreadPool::workerThread()
{
}
