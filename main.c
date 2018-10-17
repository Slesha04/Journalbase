/*******************************************************************************
 * main.c: Contains program entry point.
 *
 * Authors: Slesha Mishra, Riza Tolentino, Miles Burchell
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"

/*******************************************************************************
 * print_welcome
 * This function will print the initla program welcome message
 * inputs:
 *  none
 * outputs:
 *  none
 * Author: Miles Burchell
*******************************************************************************/
void print_welcome(void)
{
    printf("Welcome to JournalBase, secure and efficient article storage.\n");
    printf("Please select an option from the menu by entering a number from ");
    printf("the options shown.\n\n");
}

/*******************************************************************************
 * print_edit_menu
 * This function will print database editing options
 * inputs:
 *  none
 * outputs:
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void print_edit_menu(void)
{
    printf("\nYou are in editing mode. Please select an option:\n\n");
    printf("1. Add journal to the database\n");
    printf("2. Delete journal from the database\n");
    printf("3. Search for a journal\n");
    printf("4. Open a journal\n");
    printf("5. Exit\n");
}

/*******************************************************************************
 * print_read_menu
 * This function will print database reading options
 * inputs:
 *  none
 * outputs:
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void print_read_menu(void)
{
    printf("\nYou are in reading mode.\n\nPlease select an option:\n");
    printf("1. Search for a journal\n");
    printf("2. Open a journal\n");
    printf("3. Exit\n");
}

/*******************************************************************************
 * show_read_menu
 * This function will print options and take user input to perform searching and
 * viewing tasks in reading mode.
 * inputs:
 *  none
 * outputs:
 *  none
 * Authors: Riza Tolentino, Miles Burchell
*******************************************************************************/
void show_read_menu()
{
    char buffer[BUFFER_LENGTH+1];
    int no_journals, current_refid, i;

    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));

    if(head == NULL)
    {
        printf("ERROR: Out of memory.\n");
    }

    no_journals = dat_load_journal_data(head, &current_refid);

#ifdef DEBUG
    printf("DEBUG: Loaded %d journals from database.\n", no_journals);
#endif

    dat_journal_t *current = head;

    /* move current forwards by the number of journals we loaded from disk */
    for (i = 0; i < no_journals; i++)
    {
        if (!current->next)
        {
            printf("Error: Null pointer in loaded journal list. ");
            printf("Database.jb is likely corrupted on disk.\n");
            return;
        }

        current = current->next;
    }

    while(1)
    {
        print_read_menu();
        scanf("%s", buffer);
        int dat_menu_selection = atoi(buffer); /*changes into numeral*/

        /* Menu options: 1. search 2. open 3. exit */
        /*Checks for valid input*/
        if(!dat_check_menu_input(dat_menu_selection, 1, 3))
        {
            continue;
        }
        /*search through journals*/
        else if(dat_menu_selection == 1)
        {
            dat_search_journals(head, no_journals);
        }
        /*view journals to open*/
        else if (dat_menu_selection == 2)
        {
            int ref_id;

            printf("Enter journal reference no.> ");
            scanf("%d", &ref_id);

            sprintf(buffer, "%d.jb", ref_id);

            dat_open(buffer);
            remove("temp.txt");
        }
        /*End program*/
        else if(dat_menu_selection == 3)
        {
            break;
        }

    }
}

/*******************************************************************************
 * show_read_menu
 * This function will print options and take user input to perform searching,
 * viewing, adding and deleteing functions in editing mode.
 * inputs:
 *  none
 * outputs:
 *  none
 * Authors: Riza Tolentino, Miles Burchell
*******************************************************************************/
void show_edit_menu()
{
    char buffer[BUFFER_LENGTH+1];
    int no_journals, current_refid, i;

    dat_journal_t *j;
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));

    if(head == NULL)
    {
        printf("ERROR: Out of memory.\n");
    }

    no_journals = dat_load_journal_data(head, &current_refid);

#ifdef DEBUG
    printf("DEBUG: Loaded %d journals from database.\n", no_journals);
#endif

    dat_journal_t *current = head;

    /* move current forwards by the number of journals we loaded form disk */
    for (i = 0; i < no_journals; i++)
    {
        if (!current->next)
        {
            printf("Error: Null pointer in loaded journal list.");
            printf("Database.jb is likely corrupted on disk.\n");
            return;
        }

        current = current->next;
    }

    while(1)
    {
        print_edit_menu();
        scanf("%s", buffer);
        int dat_menu_selection = atoi(buffer); /* changes into numeral */

        /* Menu choices: 1. add 2. delete 3. search 4. open 5. exit */
        /* Checks for valid input */
        if(!dat_check_menu_input(dat_menu_selection, 1, 5))
        {
            continue;
        }
        /* Add a journal */
        else if(dat_menu_selection==1)
        {
            int add_success;
            /*increment number of journals*/
            no_journals++;

            /**/
            j = dat_journalentry(no_journals, &current_refid);
            current->next = j;
            j->prev = current;

            current = current->next;
            current->next = 0;

            add_success = dat_save_journal_data(head, no_journals);
            if(add_success)
            {
                no_journals --; /*the journal was not added to the database*/
                current = 0;
            }
        }
        /*Delete journals*/
        else if(dat_menu_selection == 2)
        {
            if(no_journals==0)
            {
                printf("There are no journals.\n");
            }
            else
            {
                int dat_delete_menu_selection = 0;
                int delete_key;

                dat_print_delete_menu();

                scanf("%d", &dat_delete_menu_selection);

                delete_key = dat_delete_sort(dat_delete_menu_selection, head, current_refid);

                if(delete_key != 0)
                {
                    /*a ref_id was provided to delete the journal, a journal is deleted,
                    count should decriment -- if none deleted, count will not change*/
                    no_journals = dat_delete_journal(&head, delete_key, no_journals);
                }

                dat_save_journal_data(head, no_journals);
            }
        }
        /*search through journals*/
        else if(dat_menu_selection == 3)
        {
            dat_search_journals(head, no_journals);
        }
        /*view journals to open*/
        else if (dat_menu_selection == 4)
        {
            int ref_id;

            printf("Enter journal reference no.> ");
            scanf("%d", &ref_id);

            sprintf(buffer, "%d.jb", ref_id);

            dat_open(buffer);
            remove("temp.txt");
        }
        /*End program*/
        else if(dat_menu_selection == 5)
        {
            break;
        }
    }
}

int main (int argc, char *argv[])
{
    print_welcome();

    int i;

    if (argc > 1)
    {
        for (i = 1; i < argc; i++)
        {
            if (strstr(argv[i], "-edit"))
            {
                show_edit_menu();
                return 0;
            }
        }
    }

    printf("\nIf you're looking for editing mode, launch JournalBase with ");
    printf("the -edit command line argument.\n\n");
    show_read_menu();

    return 0;
}
