#ifndef USERINPUT_H
#define USERINPUT_H

typedef enum {
    ARGV_COMMAND_GUESS = 0,
    ARGV_COMMAND_LOAD,
    ARGV_COMMAND_HELP,
    ARGV_COMMAND_EXIT,
    ARGV_COMMAND_EVAL
}argv_enum;

void evaluate_input(char* tmpbuf, bbboard* myboard);
void output_string(const char* mystring);
void process(char* tmpbuf, bbboard* myboard);
argv_enum command_line_process(char * command, int args, bbboard* myboard);

#endif // USERINPUT_H
