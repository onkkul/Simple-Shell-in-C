#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cmdline.h"


//structure to hold the parsing details
struct details
{
 	int num_tockens = 0;
 	int num_commands = 0;
 	int max_command_size = 0;
}; 


struct details calculateStatics(char ** command);
char ** parseCommand(char ** command);
int execute(char ** command);


// int run(char ** parsed_commands, int num_cmds);
int run(char ** parsed_commands, int num_cmds)
{
  	int i;
	if (strcmp(parsed_commands[0], "") == 0)
	{
	  	return 1;
	}

	if (num_cmds < 2)
	{

		for (i = 0; i < 3; i++)
		{
			if (strcmp(parsed_commands[0], "exit") == 0)
		    {
		      	printf("Cya Amigo, Leaving mysh...\n");
		      	return 0;
		    }

		    else if(strcmp(parsed_commands[0], "cd") == 0)
		    {
		    	if (parsed_commands[1] == NULL)
	  			{
			    	fprintf(stderr, "mysh: expected argument to 'cd' \n");
			  	}
			  	else
			  	{
			    	if (chdir(parsed_commands[1]) != 0)
			    	{
			      		perror("mysh");
			    	}
			  	}
			  	return 1;
		    }

		    else
		    {
		    	return execute(parsed_commands);
		    }
		}
	}
}



int main()
{	
	char ** command;

	char ** parsed_commands;
	int status;
	struct details stat;

	initCmdLine();
	while (true)
	{
		command = getCmdLine();
		stat = calculateStatics(command);
		// printf("\nparsed command has : \nnum_tockens: %d;\tnum_commands: %d;\tmax_command_size: %d;\n", stat.num_tockens, stat.num_commands, stat.max_command_size);
		
		parsed_commands = parseCommand(command);

		status = run(command, stat.num_commands);
		if (status == 0)
		{
			break;
		}
	}

	freeCmdLine();
	return 0;
}



// function to calculate parsing details
struct details calculateStatics(char ** command)
{
	struct details temp_stat;
	int max_size = 0;

	while (command[temp_stat.num_tockens] != NULL)
	{
		max_size = max_size + sizeof(command[temp_stat.num_tockens]);
		if (strcmp(command[temp_stat.num_tockens], "|") == 0 || strcmp(command[temp_stat.num_tockens], "<") == 0 || strcmp(command[temp_stat.num_tockens], ">") == 0)
		{
			temp_stat.max_command_size = fmax(temp_stat.max_command_size, max_size);
			max_size = 0;
			temp_stat.num_commands++;
		}
		temp_stat.num_tockens++;
	}
	temp_stat.max_command_size = fmax(temp_stat.max_command_size, max_size);
	max_size = 0;
	temp_stat.num_commands++;


	temp_stat.num_commands = 2*(temp_stat.num_commands) - 1;
	return temp_stat;
}



// function to pasrse the input
char ** parseCommand(char ** command)
{
	int num_tockens = 0;
	int num_cmds = 0;
	char strng[50] = "";

	char strings[100][100];

  	char **parsed_commands = (char **)malloc(sizeof(char *) * 100);

	while (command[num_tockens] != NULL)
	{
		if (strcmp(command[num_tockens],"|") == 0)
		{
			strcpy(strings[num_cmds], strng);
			parsed_commands[num_cmds] = strings[num_cmds];
			num_cmds++;
			

			strcpy(strings[num_cmds], command[num_tockens]);
			parsed_commands[num_cmds] = strings[num_cmds];
			num_cmds++;
			strcpy(strng, "");
		}
		else
		{
			strcat(strng, command[num_tockens]);
			strcat(strng, " ");
		}
		num_tockens++;
	}

	strcpy(strings[num_cmds], strng);
	parsed_commands[num_cmds] = strings[num_cmds];
	num_cmds++;

	return parsed_commands;
}

int execute(char **command)
{
	pid_t pid;
	int status;
	pid = fork();
	if(pid == 0)
	{
		if (execvp(command[0], command) == -1)
    	{
      		perror("mysh");
    	}
    	
    	exit(EXIT_FAILURE);
	}

	else if (pid < 0)
  	{
    	// Error forking
    	perror("mysh");
  	}
  	else
  	{
    	// Parent process
    	do
    	{
      		waitpid(pid, &status, WUNTRACED);
    	} 	while (!WIFEXITED(status) && !WIFSIGNALED(status));
  	}

  	return 1;

}





