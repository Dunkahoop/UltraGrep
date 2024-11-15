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

bool verbose{ false };
vector<string> extensions;

int main(int argc, char* argv[]) {
    int argIndex{ 1 };

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

    if (argv[argIndex + 2]) {
        string str = argv[argIndex + 2], item;
        stringstream in(str);

        while (getline(in, item, '.'))
            extensions.push_back("." + item);//dot added to match value of entry.path().extension()

        if (extensions.at(0) == ".")//artifact of line parsing
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

    ThreadPool pool(verbose, argv[argIndex + 1]);
    pool.start();

    // Iterate through the directory and its subdirectories
    try {
        for (const auto& entry : filesystem::recursive_directory_iterator(folder)) {
            //put file into queue if path is a regular file (lock wuth mutex to avoid corruptions)
            if (entry.is_regular_file()) {
                for (const string& ext : extensions)
                    if (entry.path().extension() == ext)
                    {
                        pool.enqueueTask(entry);
                    }
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

    pool.stop();

    cout << "Grep Report:" << endl << endl;

    for (const string& e : pool.report)
        cout << e << endl << endl;

    cout << "Total Matches: " << pool.matches << endl;

    return 0;
}

