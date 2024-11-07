// UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    bool verbose = false;
    int argIndex = 1;

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

    string folder = argv[argIndex],
        expr = argv[argIndex + 1];
    vector<string> extensions;

    // Output the values of argv
    cout << "Verbose mode: " << (verbose ? "ON" : "OFF") << endl;
    cout << "Folder: " << folder << endl;
    cout << "Expression: " << expr << endl;

    if (argv[argIndex + 2]) {
        string str = argv[argIndex + 2], item;
        stringstream in(str);

        while (getline(in, item, '.'))
        extensions.push_back(item);

        if(extensions.at(0) == "")//artifact of line parsing
        extensions.erase(extensions.begin());//removes unnecessary entry
        else// . char is required
        {
            cerr << "Extensions must be chained by \'.\' char. Example: .cpp.hpp.h" << endl;
            return 1;
        }

        cout << "Extensions: " << endl;
        for (const auto& ext : extensions) {
            cout << "\t" << ext << endl;
        }
    }

    // Iterate through the directory and its subdirectories
    try {
        for (const auto& entry : fs::recursive_directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                std::cout << "Found file: " << entry.path() << std::endl;
                // Here you can open and process the file as needed
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return 1;
    }
    
    cout << "program ended successfully" << endl;

    //if (argc < 3 || argc > 5) {
    //    cerr << "Error: inproper args" << endl;
    //    cerr << "Format:" << endl;
    //    cerr << "ultragrep [-v] folder expr [extention-list]" << endl;
    //    return 1;
    //}
    //try {
    //    

    //    if (string(argv[1]) == "-v"s) {//s makes it a string, needs to be a string got work
    //        cout << "verbose mode active" << endl;
    //        cout << "folder: " << argv[2] << endl;
    //        cout << "expression: " << argv[3] << endl;

    //        if (argv[4]) {
    //            string str = argv[4], item;
    //            vector<string> exts;
    //            stringstream in(str);

    //            while (getline(in, item, '.'))
    //                exts.push_back(item);

    //            cout << "extension(s): " << endl;
    //            for (string e : exts)
    //                cout << e << endl;
    //        }
    //    }
    //    else {
    //        cout << "folder: " << argv[1] << endl;
    //        cout << "expression: " << argv[2] << endl;

    //        if (argv[3]) {
    //            string str = argv[3], item;
    //            vector<string> exts;
    //            stringstream in(str);

    //            while (getline(in, item, '.'))
    //                exts.push_back(item);

    //            cout << "extension(s): ";
    //            for (string e : exts)
    //                cout << e << endl;
    //        }
    //    }

    //}
    //catch (invalid_argument e) {
    //    cerr << "Error: too few args -> " << e.what() << endl;
    //    return 1;
    //}
    
    return 0;
}
