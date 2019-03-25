# Lesson 2-4 (Git+Bash+Make)

## The deadline is March 25 for Part1, for Part2,3 - 1st April

#Part 1. Git
– Fork the repo (https://github.com/Kernel-GL-HRK/gl-kernel-basecamp-2019) and create your own branch;
– Implement following task:
Please, write simple game “guess a number”. User input some number from 0 to 9. Computer also generates random number from 0 to 9. If values are equal, user will get message “You win”. In other case – “You loose”.
– Create a pull request. 

Please follow flow instruction. You should focus on git interaction, not on code quality or complexity.
The goal is to create a git project with proper commit order and structure.

#Part 2. Bash

Please write a bash script that is copping all source code of your project form part1 to /tmp/guesanumber. Compress this folder to gzip archive (please google tar command) with same name. Copy gzip archive to “release” subdirectory in project dir.
Submit this script to git.

#Part 3. Make

Divide your source code form part1 into two source file. First one contains the main() function. Other one contains all other code. Write a make file for your project. 
Submit this makefile with all code to github.

