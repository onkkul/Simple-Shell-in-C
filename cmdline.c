#include "cmdline.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Global Variables */
char *cmdbuf = NULL;
size_t cmdbufsize = 0;
int numTokens = 32;
char ** tokens = NULL;

void initCmdLine()
{
	tokens = (char **)malloc(sizeof(char *) * numTokens);
}

char ** getCmdLine()
{	
	printf("mysh> ");
	// getline will reallocate cmdbuf to be large enough to fit the next line from stdin
	if (getline(&cmdbuf, &cmdbufsize, stdin) < 0)
	{
		if (feof(stdin))
		{
			strcpy(tokens[0], "exit");
			tokens[1] = NULL;
			return tokens;
		}
		perror("getCmdLine invocation of getline: ");
		tokens[0] = NULL;
		return tokens;
	}
	// Break up cmd based on white space
	int n = 0;
	char *this_token;
	while( (this_token= strsep(&cmdbuf, " \t\v\f\n\r")) !=NULL)
	{
		if (*this_token=='\0') continue;
		tokens[n] = this_token;
		n++;
		if (n >= numTokens)
		{ 	// increase the size of the tokens
			numTokens *= 2; // Double the size
			assert((tokens = (char **)realloc(tokens,sizeof(char *) * numTokens)) != NULL);
		}
	}
	tokens[n] = NULL;
	return tokens;
}

void freeCmdLine()
{
	free(tokens);
	free(cmdbuf);
}

