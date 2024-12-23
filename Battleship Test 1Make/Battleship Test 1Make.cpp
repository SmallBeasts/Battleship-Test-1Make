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
