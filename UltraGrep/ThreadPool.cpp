// ThreadPool.cpp

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(bool verbose, const string& expr)
    : stopFlag(false), numThreads(thread::hardware_concurrency()), setupDone(numThreads + 1), verbose(verbose), expr(expr), matches(0) {}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
    setupDone.count_down();
    setupDone.wait();
}

void ThreadPool::stop() {
    {
        unique_lock<mutex> lock(queueMutex);
        stopFlag = true;
    }
    wakeCond.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}

void ThreadPool::enqueueTask(const filesystem::directory_entry& entry) {
    {
        unique_lock<mutex> lock(queueMutex);
        tasks.push(entry);
    }
    wakeCond.notify_one();
}

void ThreadPool::workerThread() {
    setupDone.count_down();
    setupDone.wait();

    while (true) {
        filesystem::directory_entry entry;
        {
            unique_lock<mutex> lock(queueMutex);
            wakeCond.wait(lock, [this] { return !tasks.empty() || stopFlag; });
            if (stopFlag && tasks.empty()) {
                return;
            }
            if (!tasks.empty()) {
                entry = tasks.front();
                tasks.pop();
            }
        }

        // Process the file entry
        if(!entry.path().empty()) {
            ifstream file(entry.path());
            if (!file.is_open()) {
                cerr << "Could not open file: " << entry.path() << endl;
                continue;
            }
            string output;
            for (int count = 1; getline(file, output); count++) {
                auto words_begin = sregex_iterator(output.begin(), output.end(), expr);
                auto words_end = sregex_iterator();

                int matchCount = distance(words_begin, words_end);

                {
                    lock_guard<mutex> lk(queueMutex);
                    if (matchCount > 0) {
                        if (verbose)
                            report.push_back("Matched " + to_string(matchCount) + ": " + entry.path().string() + " [" + to_string(count) + ":" + to_string(matchCount) + "] " + output);
                        else
                            report.push_back(entry.path().string() + "\n[" + to_string(count) + "] " + output);
                    }
                    matches += matchCount;
                }
            }
        }
    }
}
