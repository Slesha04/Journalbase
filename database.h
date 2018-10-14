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
#define BUFFER_LENGTH 25
#define MAX_LENGTH_ALIAS 25
#define DAT_JOURNAL_DB_NAME "journal_info_database"
#define DB_FN "database_journal"

/*void dat_init(void);*/

int dat_add(const char* filename, const char* storename);

dat_journal_t *dat_journalentry(int no_journals);

int dat_searchjournals(dat_journal_t *head, int no_journals);

int dat_searchtitle(char search_term[], dat_journal_t *head);

int dat_searchauthor(char search_term[], dat_journal_t *head);

int dat_searchtags(char search_term[], dat_journal_t *head);

int dat_searchdate(dat_date_t search_date_term, dat_journal_t *head);

int dat_searchall(int no_journals, dat_journal_t *head);

int dat_delete_sort(int deletemenuchoice, dat_journal_t *head, int no_journals);

int dat_delete_journal( dat_journal_t **head, int key, int no_journals);

void dat_print_menu();

void dat_print_delete_menu(void);

void dat_printsearchoptions(void);

int dat_checksearchdate(int date, int month, int year);

int dat_checkword(char word[]);

int dat_check_menu_input(int menuinpu, int lowerbound, int higherbound);

int dat_save_journal_data(dat_journal_t *head, int no_journals);

int dat_load_journal_data(dat_journal_t *head);

dat_date_t dat_scan_date(void);

#endif
