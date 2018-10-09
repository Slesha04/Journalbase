#ifndef COMP_H
#define COMP_H

#include "types.h"

/* Static frequency data for huffman tree generation. \? is used to represent
   the start of plain data (not coded.) */

/* table size cannot be more than heap size */
#define COM_TABLE_SIZE 40
#define COM_HEAP_SIZE 100

/*******************************************************************************
 * This function decompresses a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int com_decompressfile(dat_file_t* file);

/*******************************************************************************
 * This function decompresses a file loaded into memory
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int com_compressfile(dat_file_t* file);

#endif
