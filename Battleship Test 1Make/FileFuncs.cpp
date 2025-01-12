#include "FileFuncs.h"
#include "Helper.h"


/* Load_File will open a file, check it and load it into the bbboard struct
    It will return the following:
    0 = Failed to open the file
    1 = Success
    -1 = File is corrupt
    -2 = Memory is allocated badly
*/
int load_file(char* filename, bbboard* myboard) {
    FILE * myfile = NULL;
    char * posbuf = NULL;
    char * filebuf = NULL;

    if (fopen_s(&myboard->savefile, filename, "r") != 0) { // Failed to open the file
        myboard->savefile = NULL;
        myboard->interaactive_go = false;
        return 0;
    }

    if (!(fscanf_s(myboard->savefile, "%i %i", &myboard->rows, &myboard->columns) == 2)) {
        fclose(myboard->savefile);
        myboard->interaactive_go = false;
        return -1;                              // Failed to read the correct two lines (rows and columns) file is corrupt
    }
    if (myboard->rows <= 0 || myboard->columns <= 0 ||
        myboard->rows > UINT16_MAX || myboard->columns > UINT16_MAX) {
        fclose(myboard->savefile);
        myboard->interaactive_go = false;
        return -1;                          // Rows/columns are too small or too large for unsigned int 16 bit
    }

    if ((INT_MAX / myboard->rows) < myboard->columns || (INT_MAX / myboard->columns) < myboard->rows) {
        fclose(myboard->savefile);              // The rows and or columns are too large.
        myboard->interaactive_go = false;
        return -1;
    }
    if (myboard->rows * myboard->columns > INT_MAX) {      // The rows and columns are too big
        fclose(myboard->savefile);
        myboard->interaactive_go = false;
        return -1;
    }
    // Dynamically allocate the memory for the array using pointer arithmetic 
    size_t total_cells = (size_t)myboard->rows * myboard->columns;
    myboard->mine = (int*)malloc(sizeof(int) * total_cells);
    if (myboard->mine == NULL) {            // Memory allocation failed
        fclose(myboard->savefile);
        myboard->interaactive_go = false;
        return -2;
    }
    int max_row_size = 7 * myboard->columns + 2;                              // Create a buffer of size 6 (unsigned int max size including -) + comma * columns + 2 for \0 and \n
    char* buf = (char*)malloc(max_row_size);                                  // Create a temporary buffer that will contain the row of data and ,'s
    if (buf == NULL) {                                                         // Memory allocation failed
        fclose(myboard->savefile);
        free(myboard->mine);
        myboard->interaactive_go = false;
        return -2;
    }
    int row_count = 0;
    while (row_count < myboard->rows) {
        if (fgets(buf, max_row_size, myboard->savefile) == NULL) {                   // It failed to read
            free(buf);                                                                       // Clean up memory
            free(myboard->mine);
            fclose(myboard->savefile);
            myboard->interaactive_go = false;
            return -1;
        }

        // Remove newline character at the end of the buffer, if present
        size_t len = strlen(buf);                                               // Unexpected blank line, file is corrupt
        if (len == 0) {
            free(buf);
            free(myboard->mine);
            fclose(myboard->savefile);
            myboard->interaactive_go = false;
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
            myboard->interaactive_go = false;
            return -1;
        }
        int col_count = 0;
        while (token != NULL) {
            if (col_count >= myboard->columns) {         // Too many tokens data is corrupt
                free(buf);
                free(myboard->mine);
                fclose(myboard->savefile);
                myboard->interaactive_go = false;
                return -1;
            }
            myboard->mine[(row_count * myboard->columns) + col_count] = strtol(token, &errptr, 10);
            if (errptr == token || *errptr != '\0') {                // The string contains no digits or non-digit characters
                fclose(myboard->savefile);
                free(buf);
                free(myboard->mine);
                myboard->interaactive_go = false;
                return -1;
            }
            token = strtok_s(NULL, ",", &context);  // Get next token
            ++col_count;
        }
        if (col_count != myboard->columns) {                 // Corrupt file, not enough tokens or too many tokens
            free(buf);
            free(myboard->mine);
            fclose(myboard->savefile);
            myboard->interaactive_go = false;
            return -1;
        }
        row_count++;
    }
    free(buf);              // Free malloc memory.
    fclose(myboard->savefile);      // Shouldn't need the file anymore for this application.  Later may want to keep it.
    myboard->loaded = 1;
    myboard->interaactive_go = true;
    return 1;
}
