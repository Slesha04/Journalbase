
#ifndef TYPES_H
#define TYPES_H

#define DEBUG

#define FALSE 0
#define TRUE 1

#define BITS_IN_BYTE 8

#define MAX_TITLE_LENGTH 256
#define MAX_KEYWORD_LENGTH 256
#define MAX_AUTHOR_NAME 256
#define MAX_NUMBER_KEYWORDS 5
#define MAX_NUMBER_AUTHORS 10
#define MAX_LENGTH_ALIAS 25
#define FILENAME_LENGTH 256
#define MAX_USER_PASS 20

/******************************************************************************
 * USER STRUCTS
 *****************************************************************************/

/* defines a user's privilege level */
typedef enum usr_priv
{
    PRIV_NONE = 0,
    PRIV_READER = 1,
    PRIV_AUTHOR = 2,
    PRIV_ADMIN = 3,
} usr_priv_t;

/* no reason not to use an array here? structure is fixed size & no need to 
   sort */

/* defines a user */
typedef struct usr_user
{
    struct usr_user* prev;
    struct usr_user* next;
    
	int user_id;
	int password_hash;
    usr_priv_t privilege;
} usr_user_t;

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
    /*char filename[MAX_TITLE_LENGTH+1];*/
    dat_date_t dat_date_dt;
    char filename[FILENAME_LENGTH + 1]; /* null terminated */
    char stored_filename[FILENAME_LENGTH + 1]; 
} dat_journal_t;

/* defines a file loaded in memory, with a pointer to the data, length in 
   bytes, and flags for compressed/encrypted */
typedef struct dat_file
{
	void* data;
	unsigned int length;
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

typedef struct log_login
{
    char first_name[MAX_USER_PASS + 1];
    char last_name[MAX_USER_PASS + 1];
    char username[MAX_USER_PASS + 1];
    char password[MAX_USER_PASS + 1];
} log_login_t;

#endif
