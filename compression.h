#ifndef COMP_H
#define COMP_H

#include "types.h"

/* table size cannot be more than heap size. if heap size is inadequate, 
   com functions will fail. */
#define COM_TABLE_SIZE 41
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
