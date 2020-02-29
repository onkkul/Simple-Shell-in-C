#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmdline.h"


// close fd after updating
void closeFD(int FD)
{
	do
	{
		int fd_to_close = (FD);
		if (close(fd_to_close) == -1)
		{
			perror("mysh>");
		}
	}while(0);
}																				


// move oldfd to newfd
void updateFD(int oldfd, int newfd)
{
	if (oldfd != newfd)
	{
		if (dup2(oldfd, newfd) != -1)
			closeFD(oldfd); 	// updateFD successful
		else
		{	
			perror("mysh>");
			exit(EXIT_FAILURE);
		}
	}
}


// get the next command i.e command2 in "command1 | command2" 
char ** getNextCommand(char ** commands)
{
	int i = 0;
	char **next = NULL;
	while(commands[i]!=NULL)
	{
		if ((strcmp(commands[i], "|") == 0))
		{

			commands[i] = NULL;
			next = &commands[i]+1;
			if (next == NULL)
			{
				perror("mysh>");
				exit(EXIT_FAILURE);
			}
			break;
		}
		i++;
	}
	return next;
}


// check if redirection inbetween tow pipes i.e "| command1 < command2 |"
// if redirection exists, it will give us locations of "<" and ">" in a command in loc[] array
int isRedirection(char ** commands, char **io, int loc[])
{
	int i = 0;
	int j = 0;
	while((commands[i] != NULL))
	{
		if ((strcmp(commands[i], ">") == 0))
		{
			loc[j] = i;
			j++;
		}
		if(strcmp(commands[i], "<") == 0)
		{
			loc[j] = i;
			j++;
		}
	i++;
	}
	return j;
}


// cat m.txt | sort > n.txt | grep 5 < n.txt
// to execute "cmd1 < cmd2" or "cmd1 > cmd2"
int runRedirection(char ** commands, int j, int loc[])
{
	int i = 0;
	int in, out;
	char ** end = commands;
	for (i; i<j; i++)
	{
		if ((strcmp(commands[loc[i]], ">")) == 0 && (strcmp(commands[loc[i]], "|") != 0)) //output
		{
			end[loc[i]] = NULL;
			out = open(commands[loc[i]+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out, 1);
			close(out);
			if (execvp(end[0], end) == -1);
			{	
				perror("mysh");
				exit(EXIT_FAILURE);
			}
		}
		else if ((strcmp(commands[loc[i]], "<") == 0) && (strcmp(commands[loc[i]], "|") != 0)) // input
		{
			in = open(commands[loc[i]+1], O_RDONLY);
   			dup2(in, 0);
   			close(in);
   			end[loc[i]] = NULL;
			if (execvp(end[0], end) == -1);
			{	
				perror("mysh");
				exit(EXIT_SUCCESS);
			}
		}
		else
		{
			perror("mysh>");
			exit(EXIT_FAILURE);
		}
		end = &end[loc[i]];
	}

	return 1;
}


// treat everything as a pipe.
// if no pipe "|" exists, pipe input is screen and output is also screen
int runPipe(char** commands, int in_fd)
{
	char ** next = NULL;
	char ** io;
	int io_loc[50];
	int redirection_count;

	next = getNextCommand(commands);

	// last command or only single command input
	if (next == NULL)
	{	
		io = commands;
		redirection_count = isRedirection(commands, io, io_loc);
		updateFD(in_fd, STDIN_FILENO); /* read from in_fd, write to STDOUT */
		if (redirection_count > 0)
		{
			runRedirection(commands, redirection_count, io_loc);
		}
		else
		{
				if (execvp(commands[0], commands) == -1)
				{
					perror("mysh");
				}
				exit(EXIT_FAILURE);
				return 1;
		}
	}

	else
	{
		// first command | middle command | last command
		// it is a pipe

		int fd[2];	// pipe
		if (pipe(fd) == -1)
		{	
			perror("mysh>");
			exit(EXIT_FAILURE);
		}

		int pid = fork();
		if (pid == 0)
		{
			// close unused pipes
			closeFD(fd[0]); 
			updateFD(in_fd, STDIN_FILENO);  // read from in_fd // STDIN_FILENO = 0
			updateFD(fd[1], STDOUT_FILENO); // write to fd[1] // STDOUT_FILENO = 1
			io = commands;
			redirection_count = isRedirection(commands, io, io_loc);
			if (redirection_count > 0)
			{
				runRedirection(commands, redirection_count, io_loc);
			}
			else
			{
				if (execvp(commands[0], commands) == -1)
				{
					perror("mysh");
					exit(EXIT_FAILURE);
				}
			}
		}

		else if (pid < 0)
		{
			perror("mysh>");
			exit(EXIT_FAILURE);
		}

		else
		{
			int status;    
    		waitpid(pid, &status, 0);
			closeFD(fd[1]); // close unused pipes
			closeFD(in_fd); 
			return runPipe(next, fd[0]); 	// recursively execute the pipe
		}
		return 1;
	}

	return 1;
}


// hosts builtins and initializes shell
int runShell(char **commands)
{
	int status;


	if (commands[0] == NULL)
	{
		status = 1;
	}

	if (strcmp(commands[0], "exit") == 0)
	{
		printf("Cya Amigo, Leaving mysh...\n");
		status = 0;
	}	
	else if(strcmp(commands[0], "cd") == 0)
	{
		if (chdir(commands[1]) != 0)
		{
			perror("mysh");
		}
		status = 1;
	}
	else
	{
		pid_t childPid = fork();
		if(childPid == 0)  // fork succeeded 
		{   
			status =  runPipe(commands, STDIN_FILENO);
		}
		else  // Main (parent) process after fork succeeds 
		{    
			int returnStatus;    
			waitpid(childPid, &returnStatus, 0);
		}
	}

	return status;
}

int main()
{	

	char ** commands = NULL;

	int status = 1;

	printf("###---Welcome to the Osh - A simple shell in C---###\nWritten by Onkar Kulkarni\nkulonku96@gmail.com\n");

	initCmdLine();
	while (status != 0)
	{
		commands = getCmdLine();

		status = runShell(commands);
		
		if (status==0)
		{
			break;
		}	
	}

	freeCmdLine();
	return 0;
}

