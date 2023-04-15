# AP_Ex1

A shell-like tool written in C is a command-line interface program that provides a user-friendly text-based interface for interacting with an operating system or custom application.

It includes features such as:
1. Redirection of input/output stream with `>`, `<`, `2>`, adding to an existing file with `>>`.
2. Change the name of the prompt with `propmt = x`, where x is the new prompt name.
3. Print out arguments with `echo`.
4. Print out the status of the last operation with `echo $?`.
5. Change the working directory of the shell with `cd x`, where x is the name of the directory.
6. Operation that repeats the last operation with `!!`.
7. Operation to exit the shell `quit`.
8. Disabeling the option to exit using `Control-C`.
9. Option to chain the input/output of operations using `|`.
10. Adding variables to the shell. `$day = Sunday`.
11. Reading operation, saving a value to a key. `read name` `x`, where x is the value for name.
12. Usage of Up/Down arrows to re-do the last operations.
13. Usage of If/Else blocks.

See examples for all cases in the examples.txt file.

## To Run
- Download the files in the repository.
- Open the folder in a Linux terminal.
- Run `make`
- Run `./myshell`
