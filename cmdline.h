#ifndef CMDLINE_H
// Required one time before running getCmdLine
void initCmdLine(); 

// Returns a list of white-space delimited tokens read from stdin
// Last element in the list is a pointer to NULL
// Results from previous call are invalidated by a new call to getCmdLine
char ** getCmdLine();

// Required one time before exiting
void freeCmdLine();

#define CMDLINE_H
#endif
