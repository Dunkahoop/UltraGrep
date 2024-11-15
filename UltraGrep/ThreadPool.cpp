#include "ThreadPool.hpp"

ThreadPool::ThreadPool(bool verbose) : stopFlag(false), verbose(verbose), setupDone(numThreads + 1) {}

ThreadPool::~ThreadPool() { stop(); }

void ThreadPool::start()
{
    stopFlag = false;
    for (unsigned i = 0; i < thread::hardware_concurrency(); ++i) {
        workers.emplace_back();
    }
    setupDone.count_down();
    setupDone.wait();
}

void ThreadPool::stop() {
    { 
        std::unique_lock<std::mutex> lock(queueMutex); 
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

void ThreadPool::enqueueTask(const filesystem::directory_entry& task)
{
    {
        lock_guard<mutex> lk(queueMutex);
        tasks.push(task);
    }
    wakeCond.notify_one();//wake up a thread to process the file
}

//void ThreadPool::workerThread()
//{
//    setupDone.count_down();
//    setupDone.wait(); // Ensure all threads start together
//
//    for (;;) {
//        filesystem::directory_entry entry;
//
//        {
//            unique_lock<mutex> lk(queueMutex);
//            wakeCond.wait(lk);
//            if (stopFlag && tasks.empty()) return;
//            entry = tasks.front();
//            tasks.pop();
//        }
//
//        ifstream file(entry.path());
//        //error cehcking, check if file can't be opened
//        if (!file.is_open()) {
//            cerr << "Could not open file: " << entry.path() << endl;
//            continue;
//        }
//        {//output file name if verbose mode is on
//            lock_guard<mutex> lk(consoleMutex);
//            if (verbose) cout << "Grepping: " << entry.path() << endl;
//        }
//        string output;
//        for (int count = 0; getline(file, output); count++) {
//
//        }
//
//        if (!entry.path().empty()) {
//            
//
//            for (const string& ext : extensions) {
//                if (entry.path().extension() == ext) {
//                    
//                    
//                    for (int count = 0; getline(file, output); count++) {
//                        //set up regex iterators
//                        auto words_begin = sregex_iterator(output.begin(), output.end(), expr);
//                        auto words_end = sregex_iterator();
//
//                        //find matches with regex, 
//                        int matchCount = distance(words_begin, words_end);
//
//                        {
//                            lock_guard<mutex> lk(consoleMtx);
//                            if (matchCount > 0) {
//                                if (verbose)
//                                    report.push_back("Found " + to_string(matchCount) + " matches: " + entry.path().string() + " [" + to_string(count) + "] " + output);
//                                else
//                                    report.push_back(entry.path().string() + "\n[" + to_string(count) + "] " + output);
//                            }
//
//                            matches += matchCount;
//                        }
//                    }
//                    if (verbose) cout << endl;
//                }
//            }
//        }
//
//    }
//}