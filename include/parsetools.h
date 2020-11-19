#ifndef PARSETOOLS_H
#define PARSETOOLS_H

#include "constants.h"
#include "parsetools.h"

// Parse a command line into a list of words,
//    separated by whitespace.
// Returns the number of words
//
int split_cmd_line(char* line, char** list_to_populate);
int split_line_pipes(char* line, char** list_to_populate);
void exec(struct executeData *parseCmd,int numOfCmds);
void Parse(char ** cmds,struct executeData *parseCmd,int numOfCmds);
char* removeSpaces(char* str);

#endif
