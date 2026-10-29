# High-Performance DFS Number Puzzle Solver

C program made to find the longest possible chains of numbers connected by their first and last 2 digits
---
## key features:
1. Processes puzzles with up to 2000 elements.
2. Converts raw fragments into an adjacency list representation.
3. Sorts graph branch targets by outbound degree to prioritize long paths early in recursion.

### How to run 

Open your terminal in the project directory and compile the program:

**Note: If you do not have a local C compiler installed, you can copy and run the code in an online environment such as OnlineGDB or Compiler Explorer.**
```bash
gcc Puzzle.c -o Puzzle.exe
.\Puzzle.exe name_of_the_file.txt
