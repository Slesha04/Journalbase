#ifndef ENC_H
#define ENC_H

#include "types.h"

/* number of encryption rounds */
#define ENC_NUM_ROUNDS 128

/* sum iterator */
#define ENC_MAGIC 0x9E3779B99E3779B9

/* 256-bit key */
#define ENC_KEY1  0x68FDC5BB
#define ENC_KEY2  0x36369B9C
#define ENC_KEY3  0x051A2B24
#define ENC_KEY4  0x62393C20
#define ENC_KEY5  0x90B60D4D
#define ENC_KEY6  0x08D67DC7
#define ENC_KEY7  0x004C0025
#define ENC_KEY8  0x4D14F373

/*******************************************************************************
 * This function decrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_decryptfile(dat_file_t* file);

/*******************************************************************************
 * This function encrypts a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int enc_encryptfile(dat_file_t* file);

#endif
