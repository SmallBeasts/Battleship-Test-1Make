#ifndef FILEFUNCS_H
#define FILEFUNCS_H



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Helper.h"
#include "UserInput.h"

int load_file(char* filename, bbboard* myboard);
void close_data(File * myfile, char* buf, char* fbuf, bbboard* myboard);

#endif // !FILEFUNCS_H