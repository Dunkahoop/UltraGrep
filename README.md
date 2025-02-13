# UltraGrep

This was written as an assignment for my Advanced C++ class during the Fall 2024 semester. It recreates the Unix command-line utility `grep`.

Two big elements featured here are:

- **Multithreading**: the program uses all available cores to search files concurrently. A Thread Pool is implemented here, where the threads are kept in an infinite loop as long as unscanned files are present
- **Regex**: The program uses a regex token to determine matches in files

## Usage

The command line format is as follows:

*Anything in square brackets (`[]`) is optional. Anything in carets (`<>`) is required, non-verbatim user input*

`ultragrep [-v] <folder> <expr> [extensions]`

- `-v` activates verbose mode, which gives more detailed output from the program's searching
- `<folder>` is the file path of the folder whose files you'd like to search
- `<expr>` is the regex expression to test the files on
- `[extensions]` is used to specify certain files that will be searched. They must be chained together by periods, eg. `.cpp.hpp.h`. If absent, the prgram will only search `.txt` files
