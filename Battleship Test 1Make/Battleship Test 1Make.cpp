// Battleship Test 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Helper.h"

/* Function to allow for consistent outputting of a string so that the screen looks good.*/
void output_string(const char* mystring) {
    printf("%s\n:> ", mystring);
}

/* Load_File will open a file, check it and load it into the bbboard struct
    It will return the following:
    0 = Failed to open the file
    1 = Success
    -1 = File is corrupt
    -2 = Memory is allocated badly
*/
int load_file(char* filename, bbboard* myboard) {
    if (fopen_s(&myboard->savefile, filename, "r") != 0) { // Failed to open the file
        myboard->savefile = NULL;
        return 0;
    }

    if (!(fscanf_s(myboard->savefile, "%i %i", &myboard->rows, &myboard->columns) == 2)) {
        fclose(myboard->savefile);
        return -1;                              // Failed to read the correct two lines (rows and columns) file is corrupt
    }
    if (myboard->rows <= 0 || myboard->columns <= 0 ||
        myboard->rows > UINT16_MAX || myboard->columns > UINT16_MAX) {
        fclose(myboard->savefile);
        return -1;                          // Rows/columns are too small or too large for unsigned int 16 bit
    }

    if ((INT_MAX / myboard->rows) < myboard->columns || (INT_MAX / myboard->columns) < myboard->rows) {
        fclose(myboard->savefile);              // The rows and or columns are too large.
        return -1;
    }
    if (myboard->rows * myboard->columns > INT_MAX) {      // The rows and columns are too big
        fclose(myboard->savefile);
        return -1;
    }
    // Dynamically allocate the memory for the array using pointer arithmetic 
    size_t total_cells = (size_t)myboard->rows * myboard->columns;
    myboard->mine = (int*)malloc(sizeof(int) * total_cells);
    if (myboard->mine == NULL) {            // Memory allocation failed
        fclose(myboard->savefile);
        return -2;
    }
    int max_row_size = 7 * myboard->columns + 2;                              // Create a buffer of size 6 (unsigned int max size including -) + comma * columns + 2 for \0 and \n
    char* buf = (char*)malloc(max_row_size);                                  // Create a temporary buffer that will contain the row of data and ,'s
    if (buf == NULL) {                                                         // Memory allocation failed
        fclose(myboard->savefile);
        free(myboard->mine);
        return -2;
    }
    int row_count = 0;
    while (row_count < myboard->rows) {
        if (fgets(buf, max_row_size, myboard->savefile) == NULL) {                   // It failed to read
            free(buf);                                                                       // Clean up memory
            free(myboard->mine);
            fclose(myboard->savefile);
            return -1;
        }

        // Remove newline character at the end of the buffer, if present
        size_t len = strlen(buf);                                               // Unexpected blank line, file is corrupt
        if (len == 0) {
            free(buf);
            free(myboard->mine);
            fclose(myboard->savefile);
            return -1;
        }
        if (len > 0 && buf[len - 1] == '\n' || buf[len - 1] == '\r') {
            buf[len - 1] = '\0';  // Replace newline with null terminator
        }
        if (strlen(buf) == 0 || buf[0] == '\0') {            // Sometimes an empty buffer is sent, continue if that occurs, this happens sometimes in tokenization
            continue;
        }

        char* context = NULL;  // Context for strtok_s
        char* token = strtok_s(buf, ",", &context);  // Tokenize using strtok_s
        char* errptr;
        if (!token) {    // File is corrupt
            free(buf);
            free(myboard->mine);
            fclose(myboard->savefile);
            return -1;
        }
        int col_count = 0;
        while (token != NULL) {
            if (col_count >= myboard->columns) {         // Too many tokens data is corrupt
                free(buf);
                free(myboard->mine);
                fclose(myboard->savefile);
                return -1;
            }
            myboard->mine[(row_count * myboard->columns) + col_count] = strtol(token, &errptr, 10);
            if (errptr == token || *errptr != '\0') {                // The string contains no digits or non-digit characters
                fclose(myboard->savefile);
                free(buf);
                free(myboard->mine);
                return -1;
            }
            token = strtok_s(NULL, ",", &context);  // Get next token
            ++col_count;
        }
        if (col_count != myboard->columns) {                 // Corrupt file, not enough tokens or too many tokens
            free(buf);
            free(myboard->mine);
            fclose(myboard->savefile);
            return -1;
        }
        row_count++;
    }
    free(buf);              // Free malloc memory.
    fclose(myboard->savefile);      // Shouldn't need the file anymore for this application.  Later may want to keep it.
    myboard->loaded = 1;
    return 1;
}


/* query_array, will query and create a string with the output of each input*/
/* input types can be A1,A2,A3,B4  or A1*/
void query_array(bbboard* myboard, char* opbuf, int array_choice = 1) {
    char* test_char = opbuf;
    char buf[LARGE_BUF_SIZE] = { 0 };  // Large buffer for output
    char tmpbuf[100];

    while (test_char != NULL && *test_char != '\0') {
        // Skip leading spaces
        while (*test_char == ' ') ++test_char;

        int row_index = 0;
        int col_index = 0;
        char row_buf[1270] = { 0 };
        char col_buf[50] = { 0 };
        int valid_count = 0, col_valid_count = 0;

        // Parse row (letters)
        while (*test_char >= 'a' && *test_char <= 'z') {
            row_buf[valid_count++] = *test_char;
            row_index = row_index * 26 + (toupper(*test_char) - 'A');
            ++test_char;
        }

        // Ensure at least one letter for row
        if (valid_count == 0) {
            sprintf_s(tmpbuf, sizeof(tmpbuf), "Invalid format: expected letters for row, found '%s',", row_buf);
            strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',') ++test_char;
            continue;
        }

        // Parse column (digits)
        while (*test_char >= '0' && *test_char <= '9') {
            col_buf[col_valid_count++] = *test_char;
            col_index = col_index * 10 + (*test_char - '0');  // Accumulate column value
            ++test_char;
        }

        // Apply zero-based indexing after parsing the full number
        if (col_valid_count > 0) {
            col_index -= 1;  // Convert to zero-based index
        }

        // Ensure at least one digit for column
        if (col_valid_count == 0) {
            sprintf_s(tmpbuf, sizeof(tmpbuf), "Invalid format: expected digits for column, found '%s',", col_buf);
            strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',') ++test_char;
            continue;
        }

        // Bounds checking
        if (row_index >= myboard->rows || col_index < 0 || col_index >= myboard->columns) {
            sprintf_s(tmpbuf, sizeof(tmpbuf), "OOB: row %s (%d) col %s (%d),", row_buf, row_index, col_buf, col_index);
            strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',') ++test_char;
            continue;
        }

        // Array arithmetic and output
        int location = row_index * myboard->columns + col_index;
        sprintf_s(tmpbuf, sizeof(tmpbuf), "%i,", myboard->mine[location]);
        strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);

        // Skip trailing spaces and commas
        while (*test_char == ' ' || *test_char == ',') ++test_char;
    }

    // Remove trailing comma
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == ',') {
        buf[len - 1] = '\0';
    }

    output_string(buf);
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
        }
        operatorbuf[strlen(operatorbuf)] = '\0';
        result = load_file(operatorbuf, myboard);
        if (!result) {  // Failed to load the board
            char tmpoutbuf[256];
            sprintf_s(tmpoutbuf, sizeof(tmpoutbuf),
                "Unable to load the file: %s\nPlease make sure that the file is in the directory or that the whole path is present.",
                operatorbuf);
            output_string(tmpoutbuf);
            return;
        }
        if (result == -1) {
            output_string("Unable to load the file, the file appears corrupt or incorrectly formatted.");
            return;
        }
        if (result == -2) {
            output_string("There was a memory error!");
            return;
        }
        output_string("The file was loaded successfully.");
        return;
    case STATE_GUESS:
        if (!myboard->loaded) {
            output_string("Please be sure to load a file first.");
            return;
        }
        else {
            query_array(myboard, operatorbuf, BOARD_MINE);
        }
        return;
    case -1:  // Handle potential direct array address
        if (!myboard->loaded) {
            output_string("Please be sure to load a file first.");
            return;
        }

        // Validate and query the input as a grid location
        if (strlen(newbuf) > 0) {
            query_array(myboard, tmpbuf, BOARD_MINE);
        }
        else {
            output_string("Invalid input. Type --help for command usage.");
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

void evaluate_input(char* tmpbuf, bbboard* myboard) {
    char* context = NULL;                // Context for strtok_s
    char* token = NULL;                  // Token pointer
    char newbuf[MAX_INPUT];              // Buffer for command data
    memset(newbuf, '\0', MAX_INPUT);     // Initialize newbuf with null terminators

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


int main()
{
    char* buf;
    char mybuf[MAX_INPUT];
    buf = mybuf;
    output_string("Welcome to Battleship Fun\nPlease enter --help for help\n--load <filename>\n--quit to quit\nor enter a value to check the file\n:");
    bbboard myboard;                            // Declare the new board which will hold the arrays and types
    memset(&myboard, 0, sizeof(myboard));       // Clear out the myboard and fill it with only 0's.
    while (true) {
        fgets(buf, MAX_INPUT, stdin);
        evaluate_input(buf, &myboard);
    }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
