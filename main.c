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
void execPipe(struct executeData *parseCmd,int numOfCmds);
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
        if(line[0] == EOF)//exit if ctrl-d is entered
            break;
        
        int numOfCmds = split_line_pipes(line, line_words); //probably splits the command line for pipes

    //I don't think we need this line, Parse handles this --Luke
    //    int numOfCmds = split_cmd_line(line,line_words); //split command line

        struct executeData parseCmd[numOfCmds]; //store command from parse

        Parse(line_words,parseCmd,numOfCmds); //parse commands pass to exec

/*
        int pid = fork();
        if (pid == 0) {
            printf("number of commands: %d\n", numOfCmds);
            execvp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
            exit(0);
        }else {
            pid = wait(NULL);
        }*/
        
        exec(parseCmd, numOfCmds);

//        for (int i = 0; i < num_words; i++) {
//            printf("%s\n", line_words[i]);
//        }
    }
    printf("\n");

    return 0;
}

//execute command lines arguments
void exec(struct executeData *parseCmd,int numOfCmds){
    pid_t pid;

    //one input
    if(numOfCmds == 1){
        pid =fork();
        if(pid == -1){
            syserror("Not able to create a fork");
        }
        else if(pid == 0){
            execvp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
            exit(0);
        }
        else{
            pid = wait(NULL);
        }
    }
    else{
        execPipe(parseCmd, numOfCmds);
    }
}


//executes piped commands
void execPipe(struct executeData *parseCmd,int numOfCmds){
    int fds[numOfCmds-1][2];
    pid_t pid;

    //first make all the needed pipes
    for(int i = 0; i < numOfCmds - 1; i++){
        if (pipe(fds[i]) == -1)
            syserror( "Could not create a pipe" );
    }

    //then exec them
    for(int i = 0; i < numOfCmds - 1; i++){

        switch ( pid = fork() ) {
        case -1: 
            syserror("First fork failed");
            break;
        case  0: 
            if (close(0) == -1)
                syserror("Could not close stdin");
            dup(fds[i][0]);
            if (close(fds[i][0]) == -1 || close(fds[i][1]) == -1)
                syserror( "Could not close fds[i]s from first child" );
            execvp(parseCmd[i+1].cmdWrds[0], parseCmd[i+1].cmdWrds);
            syserror("Could not exec ");
            break;
        default:
            //fprintf(stderr, "The first child's pid is: %d\n", pid);
            break;
        }

        switch ( pid = fork() ) {
            case -1: 
                syserror( "Second fork failed" );
                break;
            case  0: 
                if (close(1) == -1)
                    syserror("Could not close stdout");
                dup(fds[i][1]);
                if (close(fds[i][0]) == -1 || close(fds[i][1]) == -1)
                    syserror( "Could not close fds[i]s from first child" );
                execvp(parseCmd[i].cmdWrds[0], parseCmd[i].cmdWrds);
                syserror("Could not exec ");
                break;
            default:
                //fprintf(stderr, "The second child's pid is: %d\n", pid);
                break;
        }
        
        
    }

    //finaly reap children
    for(int i = 0; i < numOfCmds - 1; i++){
        if (close(fds[i][0]) == -1)
            syserror( "Parent could not close stdin" );
        if (close(fds[i][1]) == -1)
            syserror( "Parent could not close stdout" );

        while ( wait(NULL) != -1)
            ;
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

