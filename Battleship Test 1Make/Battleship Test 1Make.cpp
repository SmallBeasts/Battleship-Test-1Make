// Battleship Test 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Helper.h"
#include "UserInput.h"
#include "FileFuncs.h"



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
        char row_buf[50] = { 0 };
        char col_buf[1270] = { 0 };
        int valid_count = 0, col_valid_count = 0;
        // Keep track of the total size of the string in buf
        size_t buf_len = 0;

        // Parse col (letters)
        while (*test_char >= 'a' && *test_char <= 'z') {
            col_buf[col_valid_count++] = *test_char;
            col_index = col_index * 26 + (toupper(*test_char) - 'A'); // Col A is 0 in 0-based logic
            ++test_char;
        }

        // Ensure at least one letter for col
        if (col_valid_count == 0) {
            snprintf(tmpbuf, sizeof(tmpbuf), "Invalid format: expected letters for col, found '%s',", row_buf);
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE) {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',') ++test_char;
            continue;
        }


        // Parse row (digits)
        while (*test_char >= '0' && *test_char <= '9') {
            row_buf[valid_count++] = *test_char;
            row_index = row_index * 10 + (*test_char - '0');  // Accumulate column value
            ++test_char;
        }

        // Apply zero-based indexing after parsing the full number
        if (valid_count > 0) {
            row_index -= 1;  // Convert to zero-based index
        }

        // Ensure at least one digit for row
        if (valid_count == 0) {
            snprintf(tmpbuf, sizeof(tmpbuf), "Invalid format: expected digits for column, found '%s',", col_buf);
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE) {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }
            while (*test_char != '\0' && *test_char != ' ' && *test_char != ',') ++test_char;
            continue;
        }

        // Bounds checking
        if (row_index >= myboard->rows || col_index < 0 || col_index >= myboard->columns) {
            snprintf(tmpbuf, sizeof(tmpbuf), "OOB: row %s (%d) col %s (%d),", row_buf, row_index, col_buf, col_index);
            if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE) {
                strcat_s(buf, LARGE_BUF_SIZE - buf_len, tmpbuf);
                buf_len += strlen(tmpbuf);
            }

            while (*test_char != '\0' && *test_char != ' ' && *test_char == ',') ++test_char;
            continue;
        }

        // Array arithmetic and output
        int location = row_index * myboard->columns + col_index;
        snprintf(tmpbuf, sizeof(tmpbuf), "%i,", myboard->mine[location]);
        if (buf_len + strlen(tmpbuf) < LARGE_BUF_SIZE) {
            strcat_s(buf, LARGE_BUF_SIZE, tmpbuf);
            buf_len += strlen(tmpbuf);
        }


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



int main(int argc, char *argv[])
{
    char mybuf[MAX_INPUT];
    int args_in = 0;
    char* current = mybuf;                              // Pointer for snprintf
    size_t remaining = MAX_INPUT;               // size_t defining the remaining room in the string
    if (argc < 2) {
        output_string("Loaded with too few or no arguments.  Proceeding.");
    }
    else {
        args_in = 1;
        memset(mybuf, '\0', MAX_INPUT);         // Initialize the string
        
        for (int i = 1; i < argc; ++i) {        // Concatenate argv into a string for evaluate_input
            int written = snprintf(current, remaining, "%s%s", argv[i], (i < argc - 1) ? " " : "");
            if (written < 0 || (size_t)written >= remaining) {
                output_string("The arguments are too long to fit in the string.");
                return 1;
            }
            current += written;
            remaining -= written;
        }

    }
    if (!args_in) {
        output_string("Welcome to Battleship Fun\nPlease enter --help for help\n--load <filename>\n--quit to quit\nor enter a value to check the file\n:");
    }
    bbboard myboard;                            // Declare the new board which will hold the arrays and types
    memset(&myboard, 0, sizeof(myboard));       // Clear out the myboard and fill it with only 0's.
    myboard.guess_placed = false;
    
    while (true) {
        if (!args_in) {                         // No arguments in the command line
            fgets(mybuf, MAX_INPUT, stdin);
            evaluate_input(mybuf, &myboard);
        }
        else {
            evaluate_input(mybuf, &myboard);
            args_in = 0;                        // Future loops will no longer be checking for file input
            if (myboard.guess_placed) {
                break;
            }
        }
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
