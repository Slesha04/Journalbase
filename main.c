#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"

int main(void)
{
    dat_file_t testfile;
    testfile.compressed = 0;
    testfile.encrypted = 0;
    testfile.data = malloc(128);

    strcpy(testfile.data, 
        "Test data to compress, hopefully it will reduce the size in memory.");
    
    testfile.length = strlen(testfile.data + 1);

    com_compressfile(&testfile);

    return 0;
}