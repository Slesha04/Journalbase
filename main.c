#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"
#include "encryption.h"
#include "database.h"
#include "login.h"

int main(void)
{
	printf("YEET...on the street??\n");

	log_main_menu();

 	/*database testing*/
    int dat_menu_selection=0; 
    char choice_buffer[25+1];
    int total_journal_count = 0, relative_count =0;
    dat_journal_t *j;
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));

    if(head == NULL)
    {
	   	printf("No memory allocated\n");	
	}

	dat_journal_t *current = head;
	head->next = NULL;

	do{

	dat_print_menu();
	scanf("%s", choice_buffer);

	int dat_menu_selection = atoi(choice_buffer);

	if(dat_check_menu_input(dat_menu_selection, 1, 4)==FALSE)
	{
		continue;
	}

	if(dat_menu_selection==1)
	{
		total_journal_count++;
		relative_count++;

		j= dat_journalentry(total_journal_count);
		current->next=malloc(sizeof(dat_journal_t));
		current->next= j;
		current->next->next = NULL;

		current = current->next;

	}

	if(dat_menu_selection == 2)
	{
		if(relative_count==0)
		{
			printf("There are no journals\n");
		}
		else
		{
			int dat_delete_menu_selection = 0;
			int delete_key;

			dat_print_delete_menu();
			scanf("%d", &dat_delete_menu_selection);
			delete_key = dat_delete_sort(dat_delete_menu_selection, head, total_journal_count);
			if(delete_key != 0)
			{
				relative_count = dat_delete_journal(&head, delete_key, relative_count);
			}
		}
	}

	if(dat_menu_selection == 3)
	{
		dat_searchjournals(head, relative_count);
	}

	if(dat_menu_selection == 4)
	{
		break;
	}

	}while(dat_menu_selection !=4);

    return 0;
}