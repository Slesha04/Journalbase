
#ifndef TYPES_H
#define TYPES_H

#define _DEBUG

#define FALSE 0
#define TRUE 1

#define MAX_TITLE_LENGTH 256
#define MAX_AUTHORS_LENGTH 256
#define MAX_KEYWORD_LENGTH 32
#define MAX_KEYWORDS 16
#define FILENAME_LENGTH 8

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
    char day;
    char month;
    short year;
} dat_date_t;

/* we can do this with fixed size char[]s in memory but when saving to disk
   we will need to be much smarter with our memory usage. writing all 256 
   bytes of buffer will seriously bloat our disk usage */

/* all files, once ingested by the program, are assigned a randomly generated
   8-character filename. */

/* defines a journal saved in the program's database */
typedef struct dat_journal
{
    struct dat_journal* prev; /* null if first in list */
    struct dat_journal* next; /* null if last in list */

	  char title[MAX_TITLE_LENGTH + 1]; /* null terminated */
    char authors[MAX_AUTHORS_LENGTH + 1]; /* null terminated */
    char keywords[MAX_KEYWORD_LENGTH + 1][MAX_KEYWORDS]; /* null terminated */
    dat_date_t date;

    char filename[FILENAME_LENGTH + 1]; /* null terminated */
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

#endif
