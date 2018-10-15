/*******************************************************************************
 * encryption.c: Contains functions for encrypting and decrypting data via a 

 * modified XTEA algorithm.

 * modified XTEA algorithm

 * 
 * Algorithm Modifications:
 *  - 64-bit Sum
 *  - 256-bit Key

 *  - 96 Rounds

 *  - 128 Rounds - Due to key size increase
 *  - Slightly modified bit shifts - For obscurity

 *  - Modulus operation replacing bitwise AND for key iteration
 * 
 * Authors: Miles Burchell
*******************************************************************************/

#include <stdio.h>
#include "encryption.h"

/*******************************************************************************
 * enc_encrypt
 * This function encrypts a 64-bit block of data
 * inputs: 
 *  data: pointer to data to encrypt (read/write)
 * outputs: 
 *  overwrites data
 * Author: Miles Burchell
*******************************************************************************/
void enc_encrypt(enc_block_t* data)
{
    int i;
    enc_block_t sum = 0;
    __uint32_t key[8] = { ENC_KEY1, ENC_KEY2, ENC_KEY3, ENC_KEY4,
                          ENC_KEY5, ENC_KEY6, ENC_KEY7, ENC_KEY8 };
    __uint32_t temp[2];

    /* split block into low and high 32-bit DWORDS */
    temp[0] = (*data >> 32) & 0xFFFFFFFF;
    temp[1] = *data & 0xFFFFFFFF;

    /* perform encryption */
    for (i = 0; i < (ENC_NUM_ROUNDS / 2); i++)
    {
        temp[0] += ((temp[1] << 5 ^ temp[1] >> 6) + temp[1]) ^ 
            (sum + key[ sum % 8 ]);

        /* summation occuring between odd rounds helps protect from related-key
           attacks */
        sum += ENC_MAGIC;

        temp[1] += ((temp[0] << 5 ^ temp[0] >> 6) + temp[0]) ^ 
            (sum + key[ (sum >> 10) % 8 ]);

    }

    *data = (enc_block_t)temp[0] << 32 | temp[1];
}

/*******************************************************************************
 * enc_decrypt
 * This function decrypts a 64-bit block of data
 * inputs: 
 *  data: pointer to data to decrypt (read/write)
 * outputs: 
 *  overwrites data
 * Author: Miles Burchell
*******************************************************************************/
void enc_decrypt(enc_block_t* data)
{
    int i;
    enc_block_t sum = ENC_MAGIC * (ENC_NUM_ROUNDS / 2);
    __uint32_t key[8] = { ENC_KEY1, ENC_KEY2, ENC_KEY3, ENC_KEY4,
                          ENC_KEY5, ENC_KEY6, ENC_KEY7, ENC_KEY8 };
    __uint32_t temp[2];

/*
    temp[0] = (*data >> 32) & 0xFFFFFFFF;
    temp[1] = *data & 0xFFFFFFFF;

    for (i = 0; i < (ENC_NUM_ROUNDS / 2); i++)
    {
        temp[1] -= ((temp[0] << 4 ^ temp[0] >> 5) + temp[0]) ^ 
            (sum + key[ (sum >> 11) % 8 ]);
        
        sum -= ENC_MAGIC;

        temp[0] -= ((temp[1] << 4 ^ temp[1] >> 5) + temp[1]) ^ 
*/
    /* split block into low and high 32-bit DWORDS */
    temp[0] = (*data >> 32) & 0xFFFFFFFF;
    temp[1] = *data & 0xFFFFFFFF;

    /* perform decryption */
    for (i = 0; i < (ENC_NUM_ROUNDS / 2); i++)
    {
        temp[1] -= ((temp[0] << 5 ^ temp[0] >> 6) + temp[0]) ^ 
            (sum + key[ (sum >> 10) % 8 ]);
        
        /* summation occuring between odd rounds helps protect from related-key
           attacks */
        sum -= ENC_MAGIC;

        temp[0] -= ((temp[1] << 5 ^ temp[1] >> 6) + temp[1]) ^ 

            (sum + key[ sum % 8 ]);
    }

    *data = (enc_block_t)temp[0] << 32 | temp[1];
}

/*******************************************************************************
 * enc_decryptfile
 * This function decrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_decryptfile(dat_file_t* file)
{
    if (!file)
    {
        printf("Error: enc_decryptfile: Null pointer.\n");

        return 1;
    }

    if (!file->encrypted)
    {
        printf("Error: enc_decryptfile: File is not encrypted.\n");

        return 1;
    }

    if (file->length % 8)
    {
        printf("Error: enc_decryptfile: Length is not even 64-bit blocks.\n");

        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG: enc_decryptfile: Decrypting file length %u\n", 
        file->length);
    #endif

    int block, num_blocks = file->length / BYTES_IN_BLOCK;

    for (block = 0; block < num_blocks; block++)
    {
        enc_decrypt(&(((enc_block_t*)file->data)[block]));
    }

    file->encrypted = FALSE;

    return 0;
}

/*******************************************************************************
 * enc_encryptfile
 * This function encrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_encryptfile(dat_file_t* file)
{
    if (!file)
    {
        printf("Error: enc_encryptfile: Null pointer.\n");

        return 1;
    }

    if (file->encrypted)
    {
        printf("Error: enc_encryptfile: File is already encrypted.\n");

        return 1;
    }

    if (file->length % 8)
    {
        printf("Error: enc_encryptfile: Length is not even 64-bit blocks.\n");

        return 1;
    }

    #ifdef DEBUG
    printf("DEBUG: enc_encryptfile: Encrypting file length %u\n", 
        file->length);
    #endif

    int block, num_blocks = file->length / BYTES_IN_BLOCK;

    for (block = 0; block < num_blocks; block++)
    {
        enc_encrypt(&(((enc_block_t*)file->data)[block]));
    }

    file->encrypted = TRUE;

    return 0;
}
