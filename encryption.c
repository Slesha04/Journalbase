#include <stdio.h>
#include "encryption.h"

void enc_crypt(block_t data)
{
    
}

/*******************************************************************************
 * This function decrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_decryptfile(dat_file_t* file)
{
    if (!file->encrypted)
    {
        printf("Error: enc_decryptfile: File is not encrypted.\n");

        return 1;
    }

    /* */

    file->encrypted = FALSE;

    return 0;
}

/*******************************************************************************
 * This function encrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_encryptfile(dat_file_t* file)
{
    if (file->encrypted)
    {
        printf("Error: enc_encryptfile: File is already encrypted.\n");

        return 1;
    }

    /* */

    file->encrypted = TRUE;

    return 0;
}
