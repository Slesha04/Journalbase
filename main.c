#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"

int main(void)
{
    FILE* f = fopen("comp_test.txt", "r");

    if (!f)
    {
        printf("Couldn't open comp_test.txt\n");
        return 1;
    }

    dat_file_t testfile;

    fseek (f, 0 , SEEK_END);
    testfile.length = ftell(f);
    rewind(f);
    
    testfile.compressed = 0;
    testfile.encrypted = 0;
    testfile.data = calloc(sizeof(char), testfile.length);

    fread(testfile.data, sizeof(char), testfile.length, f);

    printf("Read %d bytes\n", testfile.length);

    fclose(f);

    com_compressfile(&testfile);

    f = fopen("compressed.txt", "w+");
    fwrite(testfile.data, sizeof(char), testfile.length, f);
    fclose(f);

    com_decompressfile(&testfile);

    f = fopen("decompressed.txt", "w+");
    fwrite(testfile.data, sizeof(char), testfile.length, f);
    fclose(f);


    free(testfile.data);

    /*printf("Data has been compressed and decompressed: %s\n", 
        (char*)testfile.data);*/

    return 0;
}