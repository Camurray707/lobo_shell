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
        if(line[0] == EOF)//exit if ctrl-d is entered
            break;
        
        int numOfCmds = split_line_pipes(line, line_words); //probably splits the command line for pipes
        struct executeData parseCmd[numOfCmds]; //store command from parse

        Parse(line_words,parseCmd,numOfCmds); //parse commands pass to exec
        exec(parseCmd, numOfCmds);
    }
    return 0;
}

