#include <stdio.h>
#include "constants.h"
#include "parsetools.h"

int main()
{
    // Buffer for reading one line of input
    char line[MAX_LINE_CHARS];
    // holds separated words based on whitespace
    char* line_words[MAX_LINE_WORDS + 1];
    
    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    while( fgets(line, MAX_LINE_CHARS, stdin) ) {


        int num_words = split_line_pipes(line, line_words); //probably splits the command line for pipes

        //int numOfCmds =split_cmd_line(line,line_words); //split command line

        struct executeData parseCmd[num_words]; //store command from parse

        Parse(line_words,parseCmd,num_words); //parse commands pass to exec

        exec(parseCmd,num_words);

//        for (int i=0; i < num_words; i++) {
//            printf("%s\n", line_words[i]);
//        }
=======
        if(line[0] == EOF)//exit if ctrl-d is entered
            break;
        
        int numOfCmds = split_line_pipes(line, line_words); //probably splits the command line for pipes
        struct executeData parseCmd[numOfCmds]; //store command from parse

        Parse(line_words,parseCmd,numOfCmds); //parse commands pass to exec
        exec(parseCmd, numOfCmds);

    }
    return 0;
}

<<<<<<< HEAD
//execute command lines arguments
void exec(struct executeData *parseCmd,int numOfCmds){
    int fds[numOfCmds-1][2];
    pid_t pid;

    if (pipe(fds) == -1)
        syserror( "Could not create a pipe" );

    //one input, tried redirection
    if(numOfCmds == 1){
        pid = fork();
        if(pid == -1){
            syserror("Not able to fork command");
        }
        if(pid == 0){
            //check in or out redirection
            if(parseCmd[0].out || parseCmd[0].in){
                if(parseCmd[0].out == 1){
                    //create new file description and set the return value to fds(lowest number of the file descriptor)
                    fds[0][1] = open(parseCmd[0].out_File,O_WRONLY | O_CREAT);
                    dup2(fds[0][1], 1);
                }

            }
            execvp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
            //syserror("Could not exec the command");

        }
    }

    for(int i =0; i < numOfCmds; i++){
        switch(pid == fork){
            case -1:
                syserror("First fork failed");
                break;
            case 0:
                dup2(fds[i][0],1);
                if (close(fds[0]) == -1 || close(fds[1]) == -1)
                    //syserror( "Could not close fds from first child" );

                execlp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
                //syserror("Could not exec the command");

            default:
                fprintf(stderr, "The first child's pid is: %d\n", pid);
                break;
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
=======

