#include <stdio.h>
#include "database.h"
#include "compression.h"
#include "encryption.h"

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
void init_dat(void)
int dat_add(const char* filename)
{
    /*fopen(filename);
    file->data = malloc(filelength);
    fread(file->data, filelength);

	dat_journal_t *j;
 
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));
 

    if(head == NULL)
    {
	    	
	   	printf("No memory allocated\n");	
	}


	dat_journal_t*current = head;

	head->next = NULL;	

    com_compressfile(file);
    enc_encryptfile(file);

    fwrite(storedname);*/
}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_add(const char* filename)
{
	/*fopen(filename);
    file->data = malloc(filelength);
    fread(file->data, filelength);
    com_compressfile(file);
    enc_encryptfile(file);
    fwrite(storedname);*/
    

}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchjournals(journal_info_t *head, int no_journals)
{
	

	if(no_journals == 0)
	{
		printf("There are no journals to search through\n");
	}

	dat_printsearchoptions();
	scanf("%s", choice_buffer);
	char choice_buffer[BUFFER_LENGTH+1];

	int search_choice = atoi(choice_buffer);
	/*CHECK MENU INPUT*/
	if(search_choice == 1)
	{
		printf("Enter the Title you are searching for>\n");

		scanf(" %[^\n]s", searchtitle);
		char *searchterm = searchtitle;
		dat_searchtitle(searchterm, head);

	}
	if(search_choice == 2)
	{
		printf("Enter an Author>\n");
		scanf(" %[^\n]s", searchauthor);
		char *searchterm = searchauthor;
		dat_searchauthor(searchterm, head);
	}
	if(search_choice == 3)
	{
		dat_date_t searchdate;

		printf("Enter the Publication Date of the Journal>\n");
		scanf(" %d%d%d", &searchdate.date, &searchdate.month, &searchdate.year);
		dat_searchdate(searchdate, head);

	}
	if(search_choice == 4)
	{
		printf("Enter a Keyword>\n");
		scanf(" %[^\n]s", searchkeyword);
		char *searchterm = searchkeyword;
		dat_searchtags(searchterm, head);

	}
	if(search_choice == 5)
	{
		dat_searchall();

	}	
}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchtitle(char* search_term, dat_journal_t *head)
{
	int i = 0;
	journal_info_t * current = head->next;

	while(current!=NULL)
		{
			if(strcmp(p_searchtitle, current->journaltitle) == 0)
			{

			i++;	
			}

			current = current->next;
		}

		if(i==0)
		{
			printf("No matches were found\n");
		}

		if(i>0)
		{
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");	
		}
	
	journal_info_t * current = head->next;
		while(current!=NULL)
		{
			if(strcmp(p_searchtitle, current->journaltitle) == 0)
			{

			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->publication_dt.date, current->publication_dt.month, 
		 	current->publication_dt.year);

			}

			current = current->next;
		}
		return 0;
}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchauthor(char* search_term, dat_journal_t *head)
{
	int i = 0;

    char searchauthor[MAX_AUTHOR_NAME];
    while(current!=NULL)
		{
			if(strcmp(search_term, current->journaltitle) == 0)
			{

			i++;	
			}

			current = current->next;
		}

		if(i==0)
		{
			printf("No matches were found\n");
		}

		if(i>0)
		{
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");	
		}
	
	journal_info_t * current = head->next;
		while(current!=NULL)
		{
			if(strcmp(searchtitle, current->journaltitle) == 0)
			{

			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->publication_dt.date, current->publication_dt.month, 
		 	current->publication_dt.year);

			}

			current = current->next;
		}

}	

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchtags(char* search_term, , journal_info_t *head)
{
	int n = 0;
	int i = 0;
	int s  = 0;
	int j = 0;
	int length1;
	journal_info_t * current = head->next;

    while(current!=NULL)
		{	
			length1=strlen(searchkeyword);
			no_keywords = current->numberofkeywords;

			for(n=0; n<no_keywords; n++, i=0)
			{
				for(j=0; j<length1; j++)
				{
					if(searchkeyword[j] == current->journalkeywords[j][n])
					{
						i++;

					}

				}
				if(i==length1)
				{
					s++;
				}
			}

			current = current->next;
		}
		if(s==0)
		{
			printf("No matches were found\n");
		}
		if(s>0)
		{
			printf("Ref. no. Title     Author     Date Published\n");
			printf("-------- --------- ---------- --------------\n");		
		}
	
		journal_info_t * current = head->next;

		while(current!=NULL)
		{
			no_keywords = current->numberofkeywords;

			for(n=0; n<no_keywords; n++, i=0)
			{
				for(j=0; j<length1; j++)
				{
					if(searchkeyword[j] == current->journalkeywords[j][n])
					{
						i++;	
					}

				}

				if(i == length1)
				{	
					printf("%-8.5d %-9.8s %-10.10s %02d %02d %04d \n", current->referenceno, 
					current->journaltitle, current->authoralias,
				 	current->publication_dt.date, current->publication_dt.month, 
				 	current->publication_dt.year);
				}
			}
			
			current = current->next;
		}


}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchdate(dat_date_t search_date_term, journal_info_t *head)
{
	int i;
	journal_info_t * current = head->next;
	
    while(current!=NULL)
		{
			if(search_date_term.date==current->publication_dt.date && search_date_term.month == current->publication_dt.month
				&& search_date_term.year == current->publication_dt.year)
			{

				i++;	
			}

			current = current->next;
		}
		if(i==0)
		{
			printf("No matches were found\n");
		}
		if(i>0)
		{
			printf("Ref. no. Title     Author     Date Published\n");
			printf("-------- --------- ---------- --------------\n");		
		}
	
		journal_info_t * current = head->next;
		while(current!=NULL)
		{
			if(search_date_term.date==current->publication_dt.date && search_date_term.month == current->publication_dt.month
				&& search_date_term.year == current ->  publication_dt.year)
			{
				
			printf("%-8.5d %-9.8s %-10.10s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->publication_dt.date, current->publication_dt.month, 
		 	current->publication_dt.year);

			}

			current = current->next;
		}
	
}

	

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
void dat_print_menu()
{

	printf("\n");
	printf("1. Add journal to the database\n");
	printf("2. Delete journal from the database\n");
	printf("3. Search for a journal\n");
	printf("4. Go back to main menu\n");
	printf("\n");

}

/*******************************************************************************
 * This function will print the database delete options
 * inputs: 
 * searchword - date
 * outputs: 
 * return TRTUE if valid, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
void printdeletemenu(void)
{
	printf("\n");
	printf("1. Enter the reference number of the journal to be deleted\n");
	printf("2. Search through the journals \n");
	printf("\n");

}

/*******************************************************************************
 * This function will ensure the datatype inputted by its users is correct
 * and lies within the suitable bounds
 * inputs: 
 * searchword - date
 * outputs: 
 * return TRTUE if valid, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
void dat_printsearchoptions(void)
{
	printf("\n");
	printf("1. Search by Title\n");
	printf("2. Search by Author\n");
	printf("3. Search by Publication Date\n");
	printf("4. Search by Keywords\n");
	printf("5. Search through all\n");
	printf("\n");
}