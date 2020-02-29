# SIMPLE SHELL in C Language

The shell is the command interpretor in an operating system such as Unix or GNU/Linux.
It is a program that executes other programs.
It provides user a command line interface to the Unix/GNU Linux system.
This project emulates Linux Shell in C language.
Though I named it Simple-Shell-in-C, it supports all the complex functionalities!!

**Used System Calls:**
1. fork()
2. dup2()
3. execvp()
4. close()
5. perror()
6. exit()

**Builtins:**
- exit
- cd
- help

**Execution Instrucitons:**
- Step 1. make clean
- Step 2. make
    or
- Step 1. gcc cmdline.c shell.c
- Step 2. ./a.out

**file descriptions:***
- cmdline.c : takes input from command line and tokenizes it for shell.c
- shell.c : main shell simulation

***In short, all that you are searching for the assignment... ;-)</br>
Star it and Spread the news!!***
