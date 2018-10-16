/*******************************************************************************
 * main.c: Contains program entry point.
 * 
 * Authors: Slesha Mishra, Riza Tolentino, Miles Burchell
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "encryption.h"
#include "database.h"

void show_journal_menu(int admin)
{
	/*database testing*/
    
    char buffer[BUFFER_LENGTH+1];
    int no_journals, current_refid;

    dat_journal_t *j;
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));

	/*scanf(" %s", buffer);

	int hash = hashpassword(buffer);

	printf("Password %s Hash %d", buffer, hash);*/

    if(head == NULL)
    {
	   	printf("ERROR: Out of memory.\n");	
	}

	no_journals = dat_load_journal_data(head, &current_refid);

	#ifdef DEBUG
	printf("DEBUG: Loaded %d journals from database.\n", no_journals);
	#endif

	dat_journal_t *current = head;

	while(1)
	{

		dat_print_menu();
		scanf("%s", buffer);
		int dat_menu_selection = atoi(buffer); /*changes into numeral*/

		/*Checks for valid input*/
		if(!dat_check_menu_input(dat_menu_selection, 1, 5))
		{
			continue;
		}

		/*Add a journal*/
		if(dat_menu_selection==1)
		{	
			/*increment number of journals*/
			no_journals++;

			/**/
			j = dat_journalentry(no_journals, &current_refid);
			current->next = j;
			j->prev = current;
			
			current = current->next;
			current->next = 0;

			dat_save_journal_data(head, no_journals);
		}

		/*Delete journals*/
		if(dat_menu_selection == 2)
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
		if(dat_menu_selection == 3)
		{
			dat_searchjournals(head, no_journals);
		}

		/*view journals to open*/
		if (dat_menu_selection == 4)
		{
			int ref_id;

			printf("Enter journal reference no.> ");
			scanf("%d", &ref_id);

			sprintf(buffer, "%d.jb", ref_id);

			dat_open(buffer);
			remove("temp.txt");
		}
		 /*End program*/
		if(dat_menu_selection == 5)
		{
			break;
		}

	} 
}

int main(void)
{
	show_journal_menu(0);
	
 	return 0;
}

