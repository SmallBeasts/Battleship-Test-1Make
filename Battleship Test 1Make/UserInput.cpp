#include "Battleship Test 1Make.h"
#include "Helper.h"
#include "FileFuncs.h"
#include "UserInput.h"


enum command_line_process(char* command, int args, bbboard* myboard) {
    // command is really argv, args is argc
    for (int i = 1; i < args; ++i) {
        if (strcmp(&command[i], "--guess") == 0) {
            // Code for guess to go here
        }
        if (strcmp(&command[i], "--load") == 0) {
            if (i + 1 > args) {
                printf("Error: Failed to load file, no filename given after load statement.\n");
                return ARGV;                                           // This should automatically return since nothing else can continue afterwards
            }
        }
        if (strcmp(&command[i], "--help") == 0) {
            printf("--help will get you this information\n--load <filename> will load a filename or give an error if the file is not found\n"
            "--quit will quit the program\nEntering a grid location with a letter followed by a number (A1) will give you the value at that grid location\n"
            "--guess followed by a set of grid locations separated by spaces will give you the results at each location.\n"
            "--exit or --quit will gracefully exit the program.");
            return false;
        }
        return false;                                                   // This is likely a guess
    }
    return true;
}


void evaluate_input(char* tmpbuf, bbboard* myboard) {
    char* context = NULL;                // Context for strtok_s
    char* token = NULL;                  // Token pointer

    // Tokenize by "--"
    token = strtok_s(tmpbuf, "--", &context);
    while (token != NULL) {
        // Remove leading/trailing spaces
        while (*token == ' ') token++;
        size_t len = strlen(token);
        while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\n')) {
            token[len - 1] = '\0';
            len--;
        }

        // Process the token which is really a new command
        if (strlen(token) > 0) {
            process(token, myboard);                  // Process the command
        }

        token = strtok_s(NULL, "--", &context);        // Get the next token
    }
}

/* Function to allow for consistent outputting of a string so that the screen looks good.*/
void output_string(const char* mystring) {
    printf("%s\n:> ", mystring);
}


void process(char* tmpbuf, bbboard* myboard) {
    char newbuf[MAX_INPUT];
    char operatorbuf[MAX_INPUT];
    int mystate = 0;
    int cmmd_size = 0;
    int result = 0;
    memset(newbuf, '\0', MAX_INPUT);
    memset(operatorbuf, '\0', MAX_INPUT);

    int c = 0;
    for (int i = 0; i < MAX_INPUT; ++i) {
        if (tmpbuf[i] == '\0') {
            break;
        }
        else {
            if (tmpbuf[i] == ' ') {  // End of the command or portion
                cmmd_size = i;
                break;
            }
            else {  // Still getting the command out
                newbuf[c] = tolower(tmpbuf[i]);
                c++;
            }
        }
    }

    // Determine command
    if (!strcmp(newbuf, "help")) {
        mystate = 0;
    }
    else if (!strcmp(newbuf, "load")) {
        mystate = STATE_LOAD;
    }
    else if (!strcmp(newbuf, "guess")) {
        mystate = STATE_GUESS;
    }
    else if (!strcmp(newbuf, "exit") || !strcmp(newbuf, "quit")) {
        output_string("Thank you for testing: Goodbye");
        exit(0);
    }
    else {
        mystate = -1;  // Unknown command, could be a direct grid location
    }

    // Parse remaining input
    if (mystate != -1) {  // Command detected
        int d = 0;
        for (int x = cmmd_size + 1; x < MAX_INPUT; ++x) {
            if (tmpbuf[x] == '\0') {
                break;
            }
            operatorbuf[d] = tmpbuf[x];
            d++;
        }
    }

    switch (mystate) {
    case 0:
        output_string("--help will get you this information\n--load <filename> will load a filename or give an error if the file is not found\n"
            "--quit will quit the program\nEntering a grid location with a letter followed by a number (A1) will give you the value at that grid location\n"
            "--guess followed by a set of grid locations separated by spaces will give you the results at each location.\n"
            "--exit or --quit will gracefully exit the program.");
        break;
    case STATE_LOAD:
        if (myboard->loaded) {  // There is already a file loaded, do a graceful cleanup and memory cleanup
            fclose(myboard->savefile);  // Close the file
            free(myboard->mine);  // Free the dynamically allocated memory
            memset(myboard, 0, sizeof(bbboard));  // Do a complete reset
            myboard->interaactive_go = false;
        }
        operatorbuf[strlen(operatorbuf)] = '\0';
        result = load_file(operatorbuf, myboard);
        if (!result) {  // Failed to load the board
            char tmpoutbuf[MAX_INPUT];
            memset(tmpoutbuf, '\0', MAX_INPUT);
            sprintf_s(tmpoutbuf, sizeof(tmpoutbuf),
                "Unable to load the file: %s\nPlease make sure that the file is in the directory or that the whole path is present.",
                operatorbuf);
            output_string(tmpoutbuf);
            return;
        }
        if (result == -1) {
            output_string("Unable to load the file, the file appears corrupt or incorrectly formatted.");
            myboard->interaactive_go = false;
            return;
        }
        if (result == -2) {
            output_string("There was a memory error!");
            myboard->interaactive_go = false;
            return;
        }
        output_string("The file was loaded successfully.");
        myboard->interaactive_go = true;                                                // File was loaded
        return;
    case STATE_GUESS:
        if (!myboard->loaded) {
            output_string("Please be sure to load a file first.");
            myboard->interaactive_go = false;
            return;
        }
        else {
            query_array(myboard, operatorbuf, BOARD_MINE);
            myboard->interaactive_go = false;                                        // A query was performed no need to go interactive
        }
        return;
    case -1:  // Handle potential direct array address
        if (!myboard->loaded) {
            output_string("Please be sure to load a file first.");
            myboard->interaactive_go = false;
            return;
        }

        // Validate and query the input as a grid location
        if (strlen(newbuf) > 0) {
            query_array(myboard, tmpbuf, BOARD_MINE);
        }
        else {
            output_string("Invalid input. Type --help for command usage.");
            myboard->interaactive_go = false;
        }
        return;
    default:
        output_string("Unknown command. Type --help for usage instructions.");
        break;
    }
}


/* Untokenized Evaluate input*/
/*
void evaluate_input(char* tmpbuf, bbboard * myboard) {
    char newbuf[MAX_INPUT];
    memset(newbuf, '\0', MAX_INPUT);                                // Set newbuf to all string termination characters
    int command_yes = 0;
    int c = 0;                                                      // Used to increment the new buffer command.
    for (int i = 0; i < MAX_INPUT; ++i) {
        if (tmpbuf[i] == '\0') {
            newbuf[c] = '\0';
            if (strlen(newbuf) > 1) {
                process(newbuf, myboard);                                    // Process the command if it is greater than 1 character
            }
            break;
        }
        if (!command_yes) {                                         // No active command found
            if (tmpbuf[i] == '-' && tmpbuf[i + 1] == '-') {         // This is the start of a command store everything until a space
                i += 1;                                            // Skip what we just checked
                command_yes = 1;
                c = 0;
                continue;
            }
        }
        else if (command_yes && tmpbuf[i] == '-' && tmpbuf[i + 1] == '-') {     // This is a new command
            process(newbuf, myboard);                                        // Process the command
            c = 0;
            memset(newbuf, '\0', MAX_INPUT);
            i++;
            continue;
        }
        else {
            if (tmpbuf[i] != ' ') {
                newbuf[c] = tmpbuf[i];
                c++;
            }
            else {
                newbuf[c] = tmpbuf[i];
                c++;
            }
        }


    }

    if (!strcmp(tmpbuf, "--help") || tmpbuf[3] == 'h') {         // Help command inserted
        printf("\n--help will get you this information\n--load <filename> will load a filename or give an error if the file is not found\n");
        printf("--quit will quit the program\nEntering a grid location with a letter followed by a number (A1) will give you the value at that grid location\n");
        printf("--guess followed by a set of grid locations separated by spaces will give you the results at each location.\n");
        return;
    }

}
*/

/* Tokenized Evaluate_input*/



