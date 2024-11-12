// UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>

using namespace std;
int matches{ 0 };
bool verbose{ false };

void outputFile(string e, filesystem::directory_entry entry) {
    int count{ 0 };
    string output;
    regex expr(e);

    if (verbose) cout << "Grepping: " << entry.path() << endl;
    ifstream file(entry.path());
    while (getline(file, output))
    {
        count++;

        /*if (output.find(expr) != string::npos) {
            istringstream stream(output); string temp;
            while (stream >> temp) {
                if (temp == expr) {
                    matches++;
                }
            }

            
        }*/

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

int main(int argc, char* argv[])
{
    
    int argIndex{ 1 };// , matches{ 0 };

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

    cout << endl;

    // Iterate through the directory and its subdirectories
    try {
        for (const auto& entry : filesystem::recursive_directory_iterator(folder)) {
            int count{ 0 };
            if (entry.is_regular_file()) {
                if (extensions.empty()) {
                    outputFile(expr, entry);
                    /*if (verbose) cout << "Grepping: " << entry.path() << endl;
                    ifstream file(entry.path());
                    while (getline(file, output))
                    {
                        count++;
                        
                        if (output.find(expr) != string::npos) {
                            istringstream stream(output); string temp;
                            while (stream >> temp) {
                                if (temp == expr) {
                                    matches++;
                                }
                            }

                            if (verbose)
                                cout << "Matched " << count  << ": " << entry.path() << " [" << count << "] " << output << endl;
                            else
                                cout << entry.path() << "\n[" << count << "] " << output << endl;
                        }

                    }
                    cout << endl;*/
                }
                else {
                    for (string ext : extensions) {
                        if (entry.path().extension() == ext) {
                            //TODO: make this a separete function, repetitive code
                            outputFile(expr, entry);
                            /*if (verbose) cout << "Grepping: " << entry.path() << endl;
                            ifstream file(entry.path());
                            while (getline(file, output))
                            {
                                count++;

                                if (output.find(expr) != string::npos) {
                                    istringstream stream(output); string temp;
                                    while (stream >> temp) {
                                        if (temp == expr) {
                                            matches++;
                                        }
                                    }

                                    if (verbose)
                                        cout << "Matched " << count << ": " << entry.path() << " [" << count << "] " << output << endl;
                                    else
                                        cout << entry.path() << "\n[" << count << "] " << output << endl;
                                }

                            }
                            cout << endl;*/
                        }
                    }
                }
            }
            else
                if (verbose) cout << "Scanning: " << entry.path() << endl;
        }
    }
    catch (const filesystem::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return 1;
    }
    
    cout << "Total Matches: " << matches << endl;
    
    return 0;
}
