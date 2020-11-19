#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "constants.h"
#include "parsetools.h"

void exec(struct executeData *parseCmd,int numOfCmds);
void syserror(const char *);

int main()
{
    // Buffer for reading one line of input
    char line[MAX_LINE_CHARS];
    // holds separated words based on whitespace
    char* line_words[MAX_LINE_WORDS + 1];

    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    while( fgets(line, MAX_LINE_CHARS, stdin) ) {
        int num_words = split_cmd_line(line, line_words); //probably splits the command line for pipes

        int numOfCmds =split_cmd_line(line,line_words); //split command line

        struct executeData parseCmd[numOfCmds]; //store command from parse

        Parse(line_words,parseCmd,numOfCmds); //parse commands pass to exec

        exec(parseCmd,numOfCmds);

        for (int i=0; i < num_words; i++) {
            printf("%s\n", line_words[i]);
        }
    }

    return 0;
}

//execute command lines arguments
void exec(struct executeData *parseCmd,int numOfCmds){
    int fds[numOfCmds-1][2];
    pid_t pid;

    //one input
    if(numOfCmds == 1){
        pid =fork();
        if(pid == -1){
            syserror("Not able to create a pipe");
        }
        if(pid == 0){


        }
    }

}



//check errors for function calls, got from pipe_demo
void syserror(const char *s)
{
    extern int errno;
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, " (%s)\n", strerror(errno));
    exit(1);
}
