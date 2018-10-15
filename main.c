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
#include "login.h"

/* by miles */
int hashpassword(const char* password)
{
	int i, hash = 10000;
	size_t len = strlen(password);

	for (i = 0; i < len; i++)
	{
		hash += password[i] ^ 123 << i;
		hash -= password[i] % 12;
	}

	return hash;
}

int main(void)
{
	printf("YEET...on the street???\n");

	/*log_main_menu();*/

 	/*database testing*/
    int dat_menu_selection=0; 
    char buffer[BUFFER_LENGTH+1];
    int total_journal_count = 0, relative_count =0;
    dat_journal_t *j;
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));

	/*scanf(" %s", buffer);

	int hash = hashpassword(buffer);

	printf("Password %s Hash %d", buffer, hash);*/

    if(head == NULL)
    {
	   	printf("No memory allocated\n");	
	}

	dat_journal_t *current = head;
	head->next = NULL;

	do{

	dat_print_menu();
	scanf("%s", buffer);

	int dat_menu_selection = atoi(buffer);

	if(dat_check_menu_input(dat_menu_selection, 1, 4)==FALSE)
	{
		continue;
	}

	if(dat_menu_selection==1)
	{
		total_journal_count++;
		relative_count++;

		j= dat_journalentry(total_journal_count);
		dat_save_journal_data(head, relative_count);
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
		int ref_id = dat_searchjournals(head, relative_count);

		if (ref_id)
		{
			ref_id += 10000;
			sprintf(buffer, "%d.jb", ref_id);
			dat_open(buffer);
			remove(buffer);
		}
	}

	if(dat_menu_selection == 4)
	{
		break;
	}

	}while(dat_menu_selection !=4);

    return 0;
}