/**
     * File Name:       UltraGrep.cpp
     * Description:     Main file for project
     * Author:			Duncan Wade
     * Date:            November 16th, 2024
*/

#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include "ThreadPool.hpp"

using namespace std;

bool verbose{ false };
vector<string> extensions;

int main(int argc, char* argv[]) {
    int argIndex{ 1 };
    auto startTime{ chrono::high_resolution_clock::now() };//used to get runtime

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

    string folder{ argv[argIndex] };

    if (argv[argIndex + 2]) {//has extensions in CLI
        string item;
        stringstream in(argv[argIndex + 2]);

        while (getline(in, item, '.'))
            extensions.push_back("." + item);//dot added to match value of entry.path().extension()

        if (extensions.at(0) == ".")//artifact of line parsing
            extensions.erase(extensions.begin());//removes unnecessary entry
        else// . char is required, artifact catches improper syntax
        {
            cerr << "Extensions must be chained by \'.\' char. Example: .cpp.hpp.h" << endl;
            return 1;
        }
    }
    else //by default it searches TXT files
        extensions.push_back(".txt");

    //set up thread pool
    ThreadPool pool(verbose, argv[argIndex + 1]);
    pool.start();

    // Iterate through the directory and its subdirectories
    try {
        for (const auto& entry : filesystem::recursive_directory_iterator(folder)) {
            //put file into queue if path is a regular file (lock with mutex to avoid corruptions)
            if (entry.is_regular_file()) {
                for (const string& ext : extensions)//ensure file has extension specified in extensions vector
                    if (entry.path().extension() == ext)
                        pool.enqueueTask(entry);
            }
            else {//put into brackets to avoid confusion: would be read as an else if otherwise
                if (verbose) cout << "Scanning: " << entry.path() << endl;
            }
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return 1;
    }

    pool.stop();

    //get final runtime
    auto endTime{ chrono::high_resolution_clock::now() };
    chrono::duration<double, std::milli> time { endTime - startTime };

    pool.generateReport(time);

    return 0;
}

