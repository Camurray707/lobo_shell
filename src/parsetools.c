#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "parsetools.h"



// Parse a command line into a list of words,
//    separated by whitespace.
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
char* removeSpaces(char* str){
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

//executeData struct will hold info to execute command, numOfCmds will hold the number of commands that are parsed
//cmds holds the splut commands
void Parse(char ** cmds,struct executeData *parseCmd, int numOfCmds){
    int totalCmdSize =0;

    // allocate memory for struct array executeData
    for(int i =0; i < numOfCmds; i++) {
        for (int j = 0; j < MAX_LINE_WORDS + 1; j++) {
            parseCmd[i].cmdWrds[j] = (char *)malloc(MAX_LINE_WORDS * sizeof(char));
        }


        //need to recognize and remove spaces
        cmds[i] = removeSpaces(cmds[i]);
        //removed white spaces and will need to store commands into the struct array
        totalCmdSize =split_cmd_line(cmds[i],parseCmd[i].cmdWrds);

        for(int j =0; j < totalCmdSize; j++){
            //compare strings, the strcmp will compare both strings and if both are identical then it will return 0.
            //Will return a + or - number depending if what string is greater or less than the other
            if(strcmp(parseCmd[i].cmdWrds[j],">")){
                //will be used as a condition in the exec in main.
                parseCmd[i].out =1;

                //copy contents from parseCmd[i].cmdWrds[j+1] into the out_file array
                strcpy(parseCmd[i].out_File,parseCmd[i].cmdWrds[j+1]);

                //decrease commands size by 2, because of the copy
                totalCmdSize-=2;

                //null the rest of the array
                parseCmd[i].cmdWrds[j] = '\0';
                parseCmd[i].cmdWrds[j+1] = '\0';
            }

        }

    }
}


//function to remove quotes


