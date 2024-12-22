/* Helper file that contains structures and constants*/


const int MAX_INPUT = 1024;
const int LARGE_BUF_SIZE = 4096;
const int STATE_LOAD = 1;
const int STATE_GUESS = 2;
const int BOARD_MINE = 1;
const int BOARD_OPP = 2;

struct bbboard {
    FILE* savefile;     // Saved file if one is present.
    int loaded;         // Check that something has indeed been loaded.
    int rows;
    int columns;
    int* mine;        // Dynamically allocate for a board for the player
    int* opp;          // Dynamically allocate for a board for the opponent
};
