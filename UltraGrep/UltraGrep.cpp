#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <thread>
#include <queue>
#include <condition_variable>
#include <latch>
#include "ThreadPool.hpp"

using namespace std;

int matches{ 0 };
bool verbose{ false };
queue<filesystem::directory_entry> files;
unsigned nThreads = 8;
bool morePossibleWork = true;
int numThreads = thread::hardware_concurrency();
vector<string> extensions, report;
regex expr;

latch setupDone{ numThreads + 1 };

mutex wakeMtx, consoleMtx, fileMtx;
condition_variable wakeCond;

void findMatch() {
    setupDone.count_down();
    setupDone.wait(); // Ensure all threads start together

    for (;;) {
        filesystem::directory_entry entry;

        {
            unique_lock<mutex> lk(wakeMtx);
            wakeCond.wait(lk, [] { return !files.empty() || !morePossibleWork; });
            if (!files.empty()) {
                entry = files.front();
                files.pop();
            }
            else if (!morePossibleWork) {
                break;
            }
        }

        if (!entry.path().empty()) {
            string output;

            for (const string& ext : extensions) {
                if (entry.path().extension() == ext) {
                    {//output file name if verbose mode is on
                        lock_guard<mutex> lk(consoleMtx);
                        if (verbose) cout << "Grepping: " << entry.path() << endl;
                    }
                    ifstream file(entry.path());
                    //error cehcking, check if file can't be opened
                    if (!file.is_open()) {
                        cerr << "Could not open file: " << entry.path() << endl;
                        continue;
                    }
                    for (int count = 0; getline(file, output); count++) {
                        //set up regex iterators
                        auto words_begin = sregex_iterator(output.begin(), output.end(), expr);
                        auto words_end = sregex_iterator();

                        //find matches with regex, 
                        int matchCount = distance(words_begin, words_end);

                        {
                            lock_guard<mutex> lk(consoleMtx);
                            if (matchCount > 0) {
                                if (verbose)
                                    report.push_back("Found " + to_string(matchCount) + " matches: " + entry.path().string() + " [" + to_string(count) + "] " + output);
                                else
                                    report.push_back(entry.path().string() + "\n[" + to_string(count) + "] " + output);
                            }

                            matches += matchCount;
                        }
                    }
                    if (verbose) cout << endl;
                }
            }
        }

    }
}


int main(int argc, char* argv[]) {
    int argIndex{ 1 };

    //setup worker threads
    vector<thread> threads;
    for (unsigned i = 0; i < numThreads; ++i) {
        threads.push_back(thread(findMatch));
    }

    setupDone.count_down();
    //setupDone.wait(); // Ensure all threads start together

    // Check if the first argument is "-v"
    if (argc > 1 && string(argv[1]) == "-v") {
        verbose = true;
        argIndex = 2; // Skip the "-v" argument
    }
    //if argc is above 3 or 4 (minimums regardless of using -v), or above 5 (maximum allowed vars), show usage
    if (argc < argIndex + 2 || argc > 5) {
        cerr << "Usage: ultragrep [-v] folder expr [extension-list]*" << endl;
        cerr << "Note: Extensions must be chained by \'.\' char. Example: .cpp.hpp.h" << endl;
        return 1;
    }

    string folder = argv[argIndex];
    expr = argv[argIndex + 1];

    if (argv[argIndex + 2]) {
        string str = argv[argIndex + 2], item;
        stringstream in(str);

        while (getline(in, item, '.'))
        extensions.push_back("." + item);//dot added to match value of entry.path().extension()

        if(extensions.at(0) == ".")//artifact of line parsing
        extensions.erase(extensions.begin());//removes unnecessary entry
        else// . char is required, artifact catches improper syntax
        {
            cerr << "Extensions must be chained by \'.\' char. Example: .cpp.hpp.h" << endl;
            return 1;
        }

        cout << "Extensions: " << endl;
        for (const auto& ext : extensions) {
            cout << "\t" << ext << endl;
        }
    }
    else {//by default it searches TXT files
        extensions.push_back(".txt");
    }

    // Iterate through the directory and its subdirectories
    try {
        for (const auto& entry : filesystem::recursive_directory_iterator(folder)) {
            //put file into queue if path is a regular file (lock wuth mutex to avoid corruptions)
            if (entry.is_regular_file()) {
                {
                    lock_guard<mutex> lk(fileMtx);
                    files.push(entry);
                }
                wakeCond.notify_one();//wake up a thread to process the file
            }
            else {
                if (verbose) cout << "Scanning: " << entry.path() << endl;
            }
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return 1;
    }

    //close work
    morePossibleWork = false;
    wakeCond.notify_all();

    //join threads, used for cleanup
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    cout << "Grep Report:" << endl << endl;

    for (string e : report)
        cout << e << endl << endl;

    cout << "Total Matches: " << matches << endl;

    return 0;
}
