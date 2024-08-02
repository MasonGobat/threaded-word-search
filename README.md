# Threaded Word Search Finder
This is a multi-threaded C program that is able to search through a given puzzle and find, if there, a given word. 

## How to Use
Using the program is simple.
1. Compile the program, no extra flags are necessary.
2. Run the program with the following arguments.
    - Puzzle
    - Word
    - Number of threads
    - Example: `./a example.txt cat 2`
3. The output will say whether or not the word was found, along with capitalizing the letters through which the word was found.

## Notes on use
- Currently, the solver does not care whether a word appears in a straight line or is snaked throughout.
- When designing your own puzzles, ensure that all letters are lowercase and that number_of_rows = number_of_columns.
    - The number of rows and columns should be the only thing on the first line of the puzzle and should be > 0.
    - Refer to the example puzzle provided if instructions remain unclear.
