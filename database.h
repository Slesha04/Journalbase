#ifndef DAT_H
#define DAT_H

#include "types.h"

#define MAX_TITLE_LENGTH 256
#define MAX_KEYWORD_LENGTH 256
#define MAX_AUTHOR_NAME 256
#define MAX_NUMBER_KEYWORDS 5
#define MAX_NUMBER_AUTHORS 10
#define FALSE 0
#define TRUE 1
#define BUFFER_LENGTH 1
#define MAX_LENGTH_ALIAS 25


void dat_init(void);

int dat_add(dat_file_t* file);

int dat_searchtitle(char* search_term);

int dat_searchauthor(char* search_term);

int dat_searchtags(char* search_term);

int dat_searchdate(dat_date_t search_term);

void dat_print_menu();

void printdeletemenu(void);

void dat_printsearchoptions(void);

#endif
