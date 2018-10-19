#ifndef DAT_H
#define DAT_H

#include "types.h"

#define DAY_MIN 1
#define DAY_MAX 31
#define MONTH_MIN 1
#define MONTH_MAX 12
#define YEAR_MIN 1
#define YEAR_MAX 2100

void dat_print_intro(void);

int dat_add(const char* filename, const char* storename);

int dat_open(const char* storename);

dat_journal_t *dat_journalentry(int no_journals, int* lastref);

int dat_search_journals(const dat_journal_t *head, int no_journals);

int dat_search_all(int no_journals, const dat_journal_t *head);

int dat_searchdate(const dat_date_t search_date_term, const dat_journal_t *head);

int dat_delete_sort(int deletemenuchoice, const dat_journal_t *head, int no_journals);

int dat_delete_journal( dat_journal_t **head, int key, int no_journals);

void dat_print_delete_menu(void);

void dat_print_search_options(void);

int dat_check_search_date(const dat_date_t);

int dat_check_word(const char word[]);

int dat_check_menu_input(int menuinpu, int lowerbound, int higherbound);

int dat_save_journal_data(dat_journal_t *head, int no_journals);

int dat_load_journal_data(dat_journal_t* head, int* lastref);

dat_date_t dat_scan_date(void);


#endif
