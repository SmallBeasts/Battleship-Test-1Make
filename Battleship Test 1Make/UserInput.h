#ifndef USERINPUT_H
#define USERINPUT_H

Typedef enum {
    ARGV_COMMAND_GUESS = 0,
    ARGV_COMMAND_LOAD,
    ARGV_COMMAND_HELP,
    ARGV_COMMAND_EXIT
}argv_enum;

void evaluate_input(char* tmpbuf, bbboard* myboard);
void output_string(const char* mystring);
void process(char* tmpbuf, bbboard* myboard);

#endif // USERINPUT_H
