/**
     * File Name:       ThreadPool.cpp
     * Description:     Function implementations of ThreadPool class
     * Author:			Duncan Wade
     * Date:            November 16th, 2024
*/

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(bool verbose, const string& expr)
    : stopFlag(false), 
    numThreads(thread::hardware_concurrency()), //sets numThreads to max threads allowed on machine
    setupDone(numThreads + 1), 
    verbose(verbose), 
    expr(expr, regex::optimize),
    matches(0), 
    matchedFiles(0) {}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    //adds threads to workers vector
    for (size_t i{ 0 }; i < numThreads; ++i)
        workers.emplace_back(&ThreadPool::workerThread, this);

    setupDone.count_down();
}

void ThreadPool::stop() {
    {//mutex to ensure all threads know work is done
        unique_lock<mutex> lock(queueMutex);
        stopFlag = true;
    }
    wakeCond.notify_all();//alerts all threads of end of work
    workers.clear();
}

void ThreadPool::enqueueTask(const filesystem::directory_entry& entry) {
    {//mutex to ensure no interference with adding task
        unique_lock<mutex> lock(taskMutex);
        tasks.push(entry);
    }
    wakeCond.notify_one();//wakes up a threads to start the work
}

void ThreadPool::generateReport(chrono::duration<double, milli> time)
{
    if (verbose) cout << "\n\n\n";//multple new lines to help separate report from verbose mode output

    cout << "Grep Report:" << endl << endl;

    for (const string& e : consoleReport)
        cout << e << endl << endl;

    cout << "Files with Matches: " << matchedFiles << endl;
    cout << "Total Matches: " << matches << endl;
    cout << "Time: " << time << endl;
}

void ThreadPool::workerThread() {
    while (true) {//infinite loop to keep threads working
        filesystem::directory_entry entry;
        {//mutex to ensure security in assigning task
            unique_lock<mutex> lock(taskMutex);
            wakeCond.wait(lock, [this] { return !tasks.empty() || stopFlag; });
            //assigns frontmost task to thread if work is still available
            if (!stopFlag || !tasks.empty()) {
                entry = tasks.front();
                tasks.pop();

                if (verbose) cout << "Scanning: " << entry.path() << endl;
            }
            else return;//breaks loop when all work is done, would hang infintely if not present
        }

        // Process the file entry
        if (!entry.path().empty()) {
            ifstream file(entry.path());
            if (!file.is_open()) {
                cerr << "Could not open file: " << entry.path() << endl;
                break;
            }

            //puts file content into string
            string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

            //skips over file if it has no regex matches at all
            if (!regex_search(fileContent, expr)) continue;
            else matchedFiles++;//good way to know how many files match by counting matching files here

            istringstream fileStream(fileContent);//can't read the file again, need to put it in stringstream to read every line
            string output;

            //process the file contents
            for (int count{ 1 }; getline(fileStream, output); count++) {
                if (regex_search(output, expr)) {//skip over lines with no regex matches
                    //use regex iterator to calculate number of regex matches
                    sregex_iterator it(output.begin(), output.end(), expr);
                    sregex_iterator end;

                    ptrdiff_t matchCount{ distance(it, end) };

                    {//mutex to ensure secure reporting
                        lock_guard<mutex> lk(consoleMutex);
                        if (verbose)
                        {//outputs message to both console and vector for report if verbose mode is on
                            string message{ "Matched " + to_string(matchCount) + ": " + entry.path().string() + " [" + to_string(count) + ":" + to_string(matchCount) + "] " + output };
                            cout << message << endl;
                            consoleReport.push_back(message);
                        }
                        else
                            consoleReport.push_back(entry.path().string() + "\n[" + to_string(count) + "] " + output);

                        matches += matchCount;
                    }
                }
            }
        }
    }
}
