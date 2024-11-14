// UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <thread>

using namespace std;
//TODO?: move all CLI vars here?
int matches{ 0 };//TODO: use locking to ensure integrity and accuracy of this count
bool verbose{ false };
vector<filesystem::directory_entry> files;
unsigned nThreads = 8;
bool morePossibleWork = true;

void outputFile(string e, filesystem::directory_entry entry) {
    int count{ 0 };
    string output;
    regex expr(e);

    if (verbose) cout << "Grepping: " << entry.path() << endl;
    ifstream file(entry.path());
    //while (getline(file, output))
    for (int count = 0; getline(file, output); count++)
    {
        //count++;

        auto words_begin = sregex_iterator(output.begin(), output.end(), expr);
        auto words_end = sregex_iterator();

        int matchCount = distance(words_begin, words_end);

        if (matchCount > 0) {
            if (verbose)
                cout << "Matched " << count << ": " << entry.path() << " [" << count << ":" << matchCount << "] " << output << endl;
            else
                cout << entry.path() << "\n[" << count << "] " << output << endl;
        }

        matches += matchCount;

    }
    cout << endl;
}
void findMatch(filesystem::directory_entry entry, string expr, vector<string> extensions) {
    for (string ext : extensions) {
        if (entry.path().extension() == ext) {
            outputFile(expr, entry);
        }
    }
}

void searchFile(string folder, string expr, vector<string> extensions) {
    for (const auto& entry : filesystem::recursive_directory_iterator(folder)) {//could be threaded
        int count{ 0 };
        if (entry.is_regular_file()) {//todo: put files in a list (vector<string>?) in a single thread, then place them in threads 1AAT, print report after all files processed
            //make this a thread?
            //use sleep-wake approach, adds a thread pool
            // make a dynamic thread pool?
            // use win32 and c++ threading?
            // use facade pattern; dont call threading directly, jsut say "do this task, do this task"
            //this will start a thread 
            files.push_back(entry);
            findMatch(entry, expr, extensions);
        }
        else
        {
            if (verbose) cout << "Scanning: " << entry.path() << endl;
        }
    }
}



int main(int argc, char* argv[])
{
    int argIndex{ 1 };

    // Check if the first argument is "-v"
    if (argc > 1 && string(argv[1]) == "-v") {
        verbose = true;
        argIndex = 2; // Skip the "-v" argument
    }

    if (argc < argIndex + 2 || argc > 5) {//if argc is below 3 or 4 (with verbose, the minimum amount needed), or greater than 5 (the maximum needed)
        cerr << "Usage: ultragrep [-v] folder expr [extension-list]*" << endl;
        cerr << "Note: Extensions must be chained by \'.\' char. Example: .cpp.hpp.h" << endl;
        return 1;
    }

    string folder = argv[argIndex], expr = argv[argIndex + 1];
    vector<string> extensions;

    // Output the values of argv
    cout << "Verbose mode: " << (verbose ? "ON" : "OFF") << endl;
    cout << "Folder: " << folder << endl;
    cout << "Expression: " << expr << endl;

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
    else {//by deafult it searches TXT files
        extensions.push_back(".txt");
    }

    cout << endl;

    // Iterate through the directory and its subdirectories
    try {
        searchFile(folder, expr, extensions);
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return 1;
    }

    //assign files to threads
    vector<thread> threads;
    /*for (unsigned i = 0; i < files.size(); ++i) {
        threads.push_back(thread(findMatch(files.at(i), expr, extensions))
    }*/
    
    cout << "Total Matches: " << matches << endl;
    
    return 0;
}
