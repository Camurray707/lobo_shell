#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "parsetools.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>



// Parse a command line into a list of words,
// Separated by whitespace.
// Returns the number of words
//
int split_cmd_line(char* line, char** list_to_populate) {
   char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
   char* delimiters = " \t\n"; // whitespace
   int i = 0;

   list_to_populate[0] = strtok_r(line, delimiters, &saveptr);

   while(list_to_populate[i] != NULL && i < MAX_LINE_WORDS - 1)  {
       list_to_populate[++i] = strtok_r(NULL, delimiters, &saveptr);
   };

   return i;
}

//Same function as cmd_line but the delimiter is et to | for pipes instead of whitespaces
int split_line_pipes(char* line, char** list_to_populate) {
    char* saveptr;  // for strtok_r; see http://linux.die.net/man/3/strtok_r
    char* delimiters = "|"; // whitespace
    int i = 0;

    list_to_populate[0] = strtok_r(line, delimiters, &saveptr);

    while(list_to_populate[i] != NULL && i < MAX_LINE_WORDS - 1)  {
        list_to_populate[++i] = strtok_r(NULL, delimiters, &saveptr);
    };

    return i;
}

//since the split cmd line function will accept s string (char *) we need to remove leading plus trailing whitespaces from the string
char* removeSpaces(char* str) {
    char* temp;
    //trim leading
    while(*str == ' ')
        str++;
    if(*str == 0){
        return str;
    }
    //trim trailing
    temp = str +strlen(str) -1;
    while(temp > str && *temp == ' '){
        temp--;
    }
    temp[1] = '\0';
    return str;
}

//function to remove quotes
void removeQuotes(char * line){
    for(int i = 0; line[i] != '\0'; i++){
        if(line[i] == '\"' || line[i] == '\''){
            line[i] = ' ';
        }
    }
}

//check errors for function calls, got from pipe_demo
void syserror(const char *s) {
    extern int errno;
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, " (%s)\n", strerror(errno));
    exit(1);
}


//executeData struct will hold info to execute command, numOfCmds will hold the number of commands that are parsed
void Parse(char** cmds,struct executeData *parseCmd, int numOfCmds) {
    int totalCmdSize = 0;

    // allocate memory for struct array executeData
    for (int i = 0; i < numOfCmds; i++) {
        for (int j = 0; j < MAX_LINE_WORDS + 1; j++) {
            parseCmd[i].cmdWrds[j] = (char *) malloc(MAX_LINE_WORDS * sizeof(char));
        }


        parseCmd[i].in = 0;
        parseCmd[i].out = 0;


        //need to recognize and remove spaces
        cmds[i] = removeSpaces(cmds[i]);
        //removed white spaces and will need to store commands into the struct array
        totalCmdSize = split_cmd_line(cmds[i], parseCmd[i].cmdWrds);


        for (int j = 0; j < totalCmdSize; j++) {
            //compare strings, the strcmp will compare both strings and if both are identical then it will return 0.
            //Will return a + or - number depending if what string is greater or less than the other
            if (!strcmp(parseCmd[i].cmdWrds[j], ">")) {
                //will be used as a condition in the exec in main.

                parseCmd[i].out = 1;

                strcpy(parseCmd[i].out_File,parseCmd[i].cmdWrds[j+1]);

                totalCmdSize-=2;
                parseCmd[i].cmdWrds[j] = '\0';
                parseCmd[i].cmdWrds[j+1] ='\0';

            }
           else if (!strcmp(parseCmd[i].cmdWrds[j], ">>")) {
                //will be used as a condition in the exec in main.


                parseCmd[i].out = 2;

                strcpy(parseCmd[i].out_File,parseCmd[i].cmdWrds[j+1]);

                totalCmdSize-=2;
                parseCmd[i].cmdWrds[j] = '\0';
                parseCmd[i].cmdWrds[j+1] ='\0';

            }
            else if (!strcmp(parseCmd[i].cmdWrds[j], "<")) {
                //will be used as a condition in the exec in main.
                parseCmd[i].in = 1;

                strcpy(parseCmd[i].in_File,parseCmd[i].cmdWrds[j+1]);
                if((j+3) >= totalCmdSize) {
                    totalCmdSize -= 2;
                    parseCmd[i].cmdWrds[j] = '\0';
                    parseCmd[i].cmdWrds[j + 1] = '\0';

                }else{
                            if(!strcmp(parseCmd[i].cmdWrds[j+2], ">")){
                                parseCmd[i].out = 1;
                                strcpy(parseCmd[i].out_File, parseCmd[i].cmdWrds[j+3]);
                                totalCmdSize -= 4;
                                parseCmd[i].cmdWrds[j] = '\0';
                                parseCmd[i].cmdWrds[j+1] = '\0';
                                parseCmd[i].cmdWrds[j+2] = '\0';
                                parseCmd[i].cmdWrds[j+3] = '\0';
                            }
                            else if(!strcmp(parseCmd[i].cmdWrds[j+2], ">>")){
                                parseCmd[i].out = 2;
                                strcpy(parseCmd[i].out_File, parseCmd[i].cmdWrds[j+3]);
                                totalCmdSize -= 4;
                                parseCmd[i].cmdWrds[j] = '\0';
                                parseCmd[i].cmdWrds[j+1] = '\0';
                                parseCmd[i].cmdWrds[j+2] = '\0';
                                parseCmd[i].cmdWrds[j+3] = '\0';
                            }
                    }
            }
        }
    }
}
/*
//execute command lines arguments
void exec(struct executeData *parseCmd,int numOfCmds) {
    int fds[numOfCmds - 1][2];
    pid_t pid;

    //one input
    if(numOfCmds == 1){
        pid =fork();
        if(pid == -1){
            syserror("Not able to create a fork");
        }
        else if(pid == 0){
            if(parseCmd[0].out || parseCmd[0].in){
                if(parseCmd[0].out== 1){
                    fds[0][1] = open(parseCmd[0].out_File, O_WRONLY | O_CREAT | O_TRUNC, 0700);//0700 is for read, write, and execute permission
                    if(fds[0][1] < 0){
                        syserror("could not open the output file");
                    }
                    dup2(fds[0][1], 1);
                }
                else if(parseCmd[0].out == 2){
                    fds[0][1] = open(parseCmd[0].out_File, O_WRONLY | O_CREAT |  O_APPEND, 0700);
                    if(fds[0][1] < 0){
                        syserror("could not open the output file");
                    }
                    dup2(fds[0][1], 1);
                }
                if(parseCmd[0].in == 3){
                    fds[0][0] = open(parseCmd[0].in_File, O_RDONLY, 0400);//0400 is for read permission only
                    if(fds[0][0] < 0){
                        syserror("could not open the input file");
                    }
                    dup2(fds[0][0], 0);
                }
                printf("here\n");
                if (close(fds[0][0]) == -1 || close(fds[0][1]) == -1)
                    syserror( "Could not close fds from here" );
            }
            execvp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
            exit(0);
        }
        else{
            if (close(fds[0][0]) == -1 || close(fds[0][1]) == -1)
                syserror( "Could not close pfds from here" );
            pid = wait(NULL);
        }
    }
    else{
        execPipe(parseCmd, numOfCmds);
    }
}
*/
//executes piped commands
void execPipe(struct executeData *parseCmd,int numOfCmds) {
    int fds[numOfCmds - 1][2];
    pid_t pid;

    //first make all the needed pipes
    for (int i = 0; i < numOfCmds; i++) {
        if (pipe(fds[i]) == -1)
            syserror("Could not create a pipe");
    }

    if (numOfCmds == 1) {
        pid = fork();
        if (pid == -1) {
            syserror("Not able to create a fork");
        }
        if (pid == 0) {
            if (parseCmd[0].out || parseCmd[0].in) {
                if (parseCmd[0].out == 1) {
                    fds[0][1] = open(parseCmd[0].out_File, O_WRONLY | O_CREAT | O_TRUNC,
                                     0700);//0700 is for read, write, and execute permission
                    if (fds[0][1] < 0) {
                        syserror("could not open the output file");
                    }
                    dup2(fds[0][1], 1);
                } else if (parseCmd[0].out == 2) {
                    fds[0][1] = open(parseCmd[0].out_File, O_WRONLY | O_CREAT | O_APPEND, 0700);
                    if (fds[0][1] < 0) {
                        syserror("could not open the output file");
                    }
                    dup2(fds[0][1], 1);
                }
                if(parseCmd[0].in == 1){
                    fds[0][0] = open(parseCmd[0].in_File, O_RDONLY, 0400);//0400 is for read permission only
                    if(fds[0][0] < 0){
                        syserror("could not open the input file");
                    }
                    dup2(fds[0][0], 0);
                }
                printf("no\n");
                if (close(fds[0][0]) == -1 || close(fds[0][1]) == -1)
                    syserror( "Could not close pfds from here" );
            }
            execvp(parseCmd[0].cmdWrds[0], parseCmd[0].cmdWrds);
            syserror("Could not exec the command");
        } else {
            if (close(fds[0][0]) == -1 || close(fds[0][1]) == -1)
                syserror("Could not close fds from here");
            while (wait(NULL) != -1);
        }
    }
    else{
    //then exec them
    for (int i = 0; i < numOfCmds; i++) {
        if (i == 0) {
            if(parseCmd[i].out){
                syserror("cannot redirect output when there is a following pipe");
            }
            switch (pid = fork()) {
                case -1:
                    syserror("First fork failed");
                    break;
                case 0:
                    dup2(fds[i][1], 1);
                    if (parseCmd[i].in) {//checking for input redirection
                        fds[i][0] = open(parseCmd[0].in_File, O_RDONLY, 0400);//0400 is for read permission only
                        if (fds[i][0] < 0) {
                            syserror("could not open the input file!!");
                        }
                        dup2(fds[i][0], 0);
                    }

                    if (close(fds[i][0]) == -1 || close(fds[i][1]) == -1)
                        syserror("Could not close fds[i]s from first child");

                    execvp(parseCmd[i].cmdWrds[0], parseCmd[i].cmdWrds);
                    syserror("Could not exec ");
                    break;
                default:
                    //fprintf(stderr, "The first child's pid is: %d\n", pid);
                    break;
            }
        } else if (i == (numOfCmds - 1)){
            if(parseCmd[i].in){
                syserror("cannot have an in redirection with a pipe coming before");
            }
            switch (pid = fork()) {
                case -1:
                    syserror("Second fork failed");
                    break;
                case 0:
                    /*
                    if (close(1) == -1)
                        syserror("Could not close stdout");
                    dup(fds[i][1]);
                     */
                    dup2(fds[i-1][0], 0);//here we are getting input from pipe

                    if (parseCmd[i].out) {
                        if (parseCmd[i].out == 1) {
                            fds[i][1] = open(parseCmd[i].out_File, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                            if (fds[i][1] < 0) {
                                syserror("could not open the output file");
                            }
                        } else if (parseCmd[i].out == 2) {
                            fds[i][1] = open(parseCmd[i].out_File, O_WRONLY | O_APPEND, 0777);
                            if (fds[i][1] < 0) {
                                syserror("could not open the output file");
                            }
                        }
                        dup2(fds[i][1], 1);
                        if (close(fds[i][0]) == -1 || close(fds[i][1]) == -1)
                            syserror("Could not close pfds from last child for pfd[i][0] or 1");
                    }
                    if (close(fds[i-1][0]) == -1 || close(fds[i-1][1]) == -1)
                        syserror("Could not close fds[i]s from first child");
                    execvp(parseCmd[i].cmdWrds[0], parseCmd[i].cmdWrds);
                    syserror("Could not exec ");
                    break;
                default:
                    //fprintf(stderr, "The second child's pid is: %d\n", pid);
                    break;
            }
        }
        else {

            switch (pid = fork()) {
                case -1:
                    syserror("Middle fork failed");
                    break;
                case 0:
                    /*
                    if (close(1) == -1)
                        syserror("Could not close stdout");
                    dup(fds[i][1]);
                     */
                    dup2(fds[i-1][0], 0);
                    dup2(fds[i][1], 1);
                    if (close(fds[i-1][0]) == -1 || close(fds[i-1][1]) == -1)
                        syserror("Could not close pfds from i-1  middle child");

                    if (close(fds[i][0]) == -1 || close(fds[i][1]) == -1)
                        syserror("Could not close fds[i]s from middle child");

                    execvp(parseCmd[i].cmdWrds[0], parseCmd[i].cmdWrds);
                    syserror("Could not exec ");
                    break;
                default:
                    //fprintf(stderr, "The second child's pid is: %d\n", pid);
                    break;

            }
        }


        //finally reap children
        if (i > 0) {
            if (close(fds[i - 1][0]) == -1) {
                syserror("Parent could not close stdin");
            }
            if (close(fds[i - 1][1]) == -1) {
                syserror("Parent could not close stdout");
            }
        }
        while (wait(NULL) != -1);

        }
    }
}

