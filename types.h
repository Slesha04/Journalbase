/*******************************************************************************
 * types.h: Contains preprocessor definitions and structures.
 * 
 * Authors: Slesha Mishra, Riza Tolentino, Miles Burchell
*******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#define DEBUG

#define FALSE 0
#define TRUE 1

#define BITS_IN_BYTE 8
#define BYTES_IN_BLOCK 8

/*#define MAX_TITLE_LENGTH 256
#define MAX_KEYWORD_LENGTH 256
#define MAX_AUTHOR_NAME 256
#define MAX_NUMBER_KEYWORDS 5
#define MAX_NUMBER_AUTHORS 10
#define MAX_LENGTH_ALIAS 25*/
#define FILENAME_LENGTH 256

#define MAX_USER_PASS 20

#define MAX_TITLE_LENGTH 256
#define MAX_KEYWORD_LENGTH 32
#define MAX_AUTHOR_NAME 50
#define MAX_NUMBER_KEYWORDS 5
#define MAX_NUMBER_AUTHORS 10

#define BUFFER_LENGTH 256

#define MAX_LENGTH_ALIAS 25

#define DAT_JOURNAL_DB_NAME "database.jb"

/******************************************************************************
 * ENCRYPTION STRUCTS
 *****************************************************************************/

/* 64 bits of data to be encrypted/decrypted */
typedef unsigned long long enc_block_t;

/******************************************************************************
 * DATABASE STRUCTS
 *****************************************************************************/

/* defines a date */
typedef struct dat_date
{
    int date;
    int month;
    int year;
} dat_date_t;

/* we can do this with fixed size char[]s in memory but when saving to disk
   we will need to be much smarter with our memory usage. writing all 256 
   bytes of buffer will seriously bloat our disk usage */

/* all files, once ingested by the program, are assigned a randomly generated
   8-character filename. */

/* defines a journal saved in the program's database */
typedef struct dat_journal
{
    struct dat_journal *prev;
    struct dat_journal *next;

    char journaltitle[MAX_TITLE_LENGTH+1];
    char authorname[MAX_AUTHOR_NAME+1][MAX_NUMBER_AUTHORS];
    char journalkeywords[MAX_KEYWORD_LENGTH+1][MAX_NUMBER_KEYWORDS];
    char authoralias[MAX_LENGTH_ALIAS];
    int numberofkeywords;
    int numberofauthors;
    int referenceno;

    dat_date_t dat_date_dt;
    char filename[FILENAME_LENGTH + 1]; /* null terminated */
    char stored_filename[FILENAME_LENGTH + 1]; 
} dat_journal_t;

/* defines a file loaded in memory, with a pointer to the data, length in 
   bytes, and flags for compressed/encrypted */
typedef struct dat_file
{
	void* data;
	unsigned int length; /* length in chars (may be rounded up to 8) */
    unsigned int real_length; /* length of actual data (not rounded up to 8) */
	char compressed;
	char encrypted;
} dat_file_t;

/******************************************************************************
 * COMPRESSION STRUCTS
 *****************************************************************************/

/* defines a node in the huffman tree */
typedef struct com_huffnode
{
    struct com_huffnode* left;
    struct com_huffnode* right;
    struct com_huffnode* up;

    char updir;

    char character;
    int frequency;

    char enc_bits[BITS_IN_BYTE + 1];
    int enc_length; /* length in bits */
} com_huffnode_t;

typedef struct com_bitstream
{
    char* buffer;
    int byte; /* current byte offset */
    int bit; /* current bit offset */
} com_bitstream_t;

/******************************************************************************
 * LOGIN STRUCTS
 *****************************************************************************/

typedef struct log_login
{
    char first_name[MAX_USER_PASS + 1];
    char last_name[MAX_USER_PASS + 1];
    char username[MAX_USER_PASS + 1];
    char password[MAX_USER_PASS + 1];
} log_login_t;

#endif
