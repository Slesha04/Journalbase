#ifndef DAT_H
#define DAT_H

#include "types.h"

#define DAY_MIN 1
#define DAY_MAX 31
#define MONTH_MIN 1
#define MONTH_MAX 12
#define YEAR_MIN 1
#define YEAR_MAX 2100
#define ENTER 10
#define SPACE 32
#define COMMA 44

int dat_add(const char* filename, const char* storename);

int dat_open(const char* storename);

dat_journal_t *dat_journalentry(int no_journals, int* lastref);

int dat_searchjournals(dat_journal_t *head, int no_journals);

int dat_searchall(int no_journals, dat_journal_t *head);

int dat_delete_sort(int deletemenuchoice, dat_journal_t *head, int no_journals);

int dat_delete_journal( dat_journal_t **head, int key, int no_journals);

void dat_print_menu(void);

void dat_print_delete_menu(void);

void dat_printsearchoptions(void);

int dat_checksearchdate(int date, int month, int year);

int dat_checkword(const char word[]);

int dat_check_menu_input(int menuinpu, int lowerbound, int higherbound);

int dat_save_journal_data(dat_journal_t *head, int no_journals);

int dat_load_journal_data(dat_journal_t* head, int* lastref);

dat_date_t dat_scan_date(void);

#endif
