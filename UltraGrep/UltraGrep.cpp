// UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        cerr << "Error: too few args" << endl;
        return 1;
    }
    try {
        if (argv[1] == "-v") {
            cout << "verbose mode active" << endl;
            cout << "folder: " << argv[2] << endl;
            cout << "expr: " << argv[3] << endl;

        }
        else {
            cout << "verbose mode inactive" << endl;
            cout << "folder: " << argv[1] << endl;
            cout << "expr: " << argv[2] << endl;
        }
    }
    catch (invalid_argument e) {
        cerr << "Error: too few args -> " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
