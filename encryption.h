#ifndef ENC_H
#define ENC_H

#include "types.h"

typedef unsigned long long block_t;

/*******************************************************************************
 * This function decrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Tegh
*******************************************************************************/
int enc_decryptfile(dat_file_t* file);

/*******************************************************************************
 * This function encrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Tegh 
*******************************************************************************/
int enc_encryptfile(dat_file_t* file);

#endif
