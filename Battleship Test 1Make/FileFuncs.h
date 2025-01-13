#ifndef FILEFUNCS_H
#define FILEFUNCS_H



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Helper.h"
#include "UserInput.h"

typedef enum {
    LF_FILE_LOADED,
    LF_FILE_NOT_FOUND,
    LF_FILE_CORRUPT,
    LF_MEMORY_FAIL
}LoadFile_enum;

LoadFile_enum load_file(char* filename, bbboard* myboard);

#endif // !FILEFUNCS_H