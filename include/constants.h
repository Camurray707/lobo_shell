#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_LINE_CHARS 1024
#define MAX_LINE_WORDS 100

#define INPUT_REDIRECT '<'
#define OUTPUT_REDIRECT '>'
#define PIPE '|'

struct executeData{
    char * cmdWrds[MAX_LINE_WORDS+1];
    int in, out;
    char* out_File[MAX_LINE_CHARS];
    char* in_File[MAX_LINE_CHARS];
};

#endif
