#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "compression.h"
#include "encryption.h"

int main(void)
{
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
/*dat_journal_t *dat_init(void)
{
	just write this in main
	dat_journal_t *j;
 
    dat_journal_t *head = NULL;
    head = malloc(sizeof(dat_journal_t));
 

    if(head == NULL)
    {
	    	
	   	printf("No memory allocated\n");	
	}


	dat_journal_t *current = head;

	head->next = NULL;	
	return j;
}
*/

int dat_add(const char* filename)
{
	FILE* journalfiletext;
	dat_file_t file;
	long filesize;
	size_t file_result;

    journalfiletext = fopen(filename, "r");
    if(journalfiletext==NULL)
    {
    	printf("Read error\n");
   		return FALSE;
    }

    /*obtain file length*/
    fseek(journalfiletext, 0, SEEK_END);
    filesize = ftell(journalfiletext);
    rewind(journalfiletext); /*returns pointer to start*/ 

    /*allocate the memory for the file*/
    file.data = (char*)malloc(sizeof(char)*filesize);
    if(file.data==NULL)
    {
    	printf("Memory error\n");
    	return FALSE;
    }

    file_result=fread(file.data,sizeof(char),filesize,journalfiletext);
    if(file_result != filesize)
    {
    	printf("Read error\n");
    }

   /*the file is loadedin the memory.
    com_compressfile(file);
    enc_encryptfile(file);
    fwrite(filename);*/

    
    fclose(journalfiletext);
    return TRUE;
}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
dat_journal_t *dat_journalentry(int no_journals)
{


	dat_journal_t *j = NULL;
	j = malloc(sizeof(dat_journal_t));
    

	int valid = TRUE, validcheck2 = TRUE, check;
	int i = 0, m = 0, k=0, n=0, x=0;
	char keywords, temp, author_buffer;
	char d_buffer[BUFFER_LENGTH], m_buffer[BUFFER_LENGTH], y_buffer[BUFFER_LENGTH];

    if(j == NULL){
	    	
	    	printf("No memory allocated\n");	
		 }

	printf("Enter the Journal information>\n");

	do{
		printf("Enter the File Name>\n");
		scanf(" %[^\n]s", (*j).filename);

		if(dat_add((*j).filename)==FALSE)
		{
			valid = FALSE;
		}

		else
		{
			valid = TRUE;
		}	
		
		}while(valid==FALSE);

	do{
		printf("Enter the Journal Title>\n");
		scanf(" %[^\n]s", (*j).journaltitle);

		if(dat_checkword((*j).journaltitle)==FALSE)
		{
			printf("Invalid Title\n");
		}

	}while(dat_checkword((*j).journaltitle)==FALSE);

	do{
		/*if the input has been previously invalid but skipped the first warning, 
		the user will now be warned*/
		if(validcheck2==FALSE)
		{
			printf("Invalid date\n");
		}

		printf("Enter the Publication Date>\n");
		
	
		scanf(" %s", d_buffer);
		check = getchar();
		if(check==10)
			{
				valid=FALSE;
				validcheck2 = FALSE;
				continue;
			}
		else
			{
				scanf("%s", m_buffer);
				check = getchar();
				if(check==10)
				{
					valid=FALSE;
					validcheck2 = FALSE;
					continue;
				}
				else
				{
					scanf("%s", y_buffer);  
				}

			}
		(*j).dat_date_dt.date = atoi(d_buffer);
		(*j).dat_date_dt.month = atoi(m_buffer);
		(*j).dat_date_dt.year = atoi(y_buffer);

		valid = (dat_checksearchdate((*j).dat_date_dt.date, 
			(*j).dat_date_dt.month, (*j).dat_date_dt.year));

		if(valid==FALSE)
		{
			printf("Invalid date\n");
		}

	}while(valid == FALSE);

	do{
		printf("Enter the Author's Name, if multiple authors, separate by a comma and space>\n");
		getchar();
		do
		{ 
			if(author_buffer==10)
			{
				n = 0;
				k = 0;
			}

			author_buffer = getchar();
			k++;

			if(author_buffer != (32||44))
			{
				(*j).authorname[k-1][n] = author_buffer;
				if(n==0 && author_buffer!=10)
				{
					(*j).authoralias[k-1] = author_buffer;
				}
			}
			if(author_buffer == 32 && (temp == 44||m==0))
			{
				(*j).authorname[k-1][n] = 0;
				k=0;
				x ++;
				n++;
			}
			/*
			if(n==0 && k>0 && x==0 && author_buffer!) the first author
			{	
				
				(*j).authoralias[k-1] = author_buffer;
				
			}
			*/
			temp = author_buffer;

		}while(author_buffer != 10);

		(*j).numberofauthors = n + 1;
		if((*j).numberofauthors>1)
		{
			strcat((*j).authoralias, "et al.");
		}

		if(i>MAX_NUMBER_AUTHORS)
		{
			valid = FALSE;
			printf("You have entered too many authors\n");
		}

	}while(valid!=TRUE);
	

	do
	{
		printf("The maximum number of keywords is 5\n");
		printf("Enter keywords, separated by a space>\n");
		
		getchar();
		do
		{ if(keywords==10)
			{
				i = 0;
				m = 0;
			}

			keywords = getchar();
			m++;

			if(keywords != (32||44))
			{
				(*j).journalkeywords[m-1][i] = keywords;

			}
			if(keywords == 32 && (temp == 44||m==0))
			{
				(*j).journalkeywords[m-1][i] = 0;
				m=0;
				i++;
			}
			
			temp = keywords;
		}while(keywords != 10);

		(*j).numberofkeywords = i + 1;
		if(i>MAX_NUMBER_KEYWORDS)
		{
			valid = FALSE;
			printf("You have entered too many keywords\n");
		}

	}while(valid!=TRUE);

	(*j).referenceno = 10000 + no_journals;
	printf("Your reference number is: %d\n", (*j).referenceno);


return j;

}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchjournals(dat_journal_t *head, int no_journals)
{
	char choice_buffer[BUFFER_LENGTH+1];
	char searchtitle[MAX_TITLE_LENGTH+1];
	char searchauthor[MAX_AUTHOR_NAME+1];
	char searchkeyword[MAX_KEYWORD_LENGTH+1];

	if(no_journals == 0)
	{
		printf("There are no journals to search through\n");
	}

	dat_printsearchoptions();
	scanf("%s", choice_buffer);


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
		scanf(" %d %d %d", &searchdate.date, &searchdate.month, &searchdate.year);
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
		dat_searchall(no_journals, head);

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
int dat_searchtitle(char* search_term, dat_journal_t *head)
{
	int i = 0;
	dat_journal_t * current = head->next;

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
	
	/*dat_journal_t * current = head->next;*/
		while(current!=NULL)
		{
			if(strcmp(search_term, current->journaltitle) == 0)
			{

			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->dat_date_dt.date, current->dat_date_dt.month, 
		 	current->dat_date_dt.year);

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
	dat_journal_t * current = head->next;
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
	
	/*dat_journal_t * current = head->next;*/
		while(current!=NULL)
		{
			if(strcmp(search_term, current->journaltitle) == 0)
			{

			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->dat_date_dt.date, current->dat_date_dt.month, 
		 	current->dat_date_dt.year);

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
int dat_searchtags(char searchkeyword[], dat_journal_t *head)
{
	int n = 0;
	int i = 0;
	int s  = 0;
	int j = 0;
	int length1;
	int no_keywords;

 dat_journal_t * current = head->next;

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
	
	 /*dat_journal_t * current = head->next;*/

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
				 	current->dat_date_dt.date, current->dat_date_dt.month, 
				 	current->dat_date_dt.year);
				}
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
int dat_searchdate(dat_date_t search_date_term, dat_journal_t *head)
{
	int i;
 dat_journal_t * current = head->next;
	
    while(current!=NULL)
		{
			if(search_date_term.date==current->dat_date_dt.date && search_date_term.month == current->dat_date_dt.month
				&& search_date_term.year == current->dat_date_dt.year)
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
	
	 /*dat_journal_t * current = head->next;*/
		while(current!=NULL)
		{
			if(search_date_term.date==current->dat_date_dt.date && search_date_term.month == current->dat_date_dt.month
				&& search_date_term.year == current ->  dat_date_dt.year)
			{
				
			printf("%-8.5d %-9.8s %-10.10s %02d %02d %04d \n", current->referenceno, 
			current->journaltitle, current->authoralias,
		 	current->dat_date_dt.date, current->dat_date_dt.month, 
		 	current->dat_date_dt.year);

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
int dat_searchall(int no_journals, dat_journal_t *head)
{
	dat_journal_t * current = head->next;
			if(no_journals==0)
		{
			printf("No journals on the database\n");
			
		}
		else
		{
 
			printf("Ref. no. Title     Author     Date Published\n");
			printf("-------- --------- ---------- --------------\n");	
			
			while(current != NULL)
			{
			
				printf("%-8.5d %-9.8s %-10.10s %02d %02d %04d \n", current->referenceno, 
				current->journaltitle, current->authoralias,
			 	current->dat_date_dt.date, current->dat_date_dt.month, 
			 	current->dat_date_dt.year);

		 		current = current->next;

			}
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
int dat_delete_sort(int deletemenuchoice, dat_journal_t *head, int no_journals)
{

	int delete_ref_key;
	

	if(deletemenuchoice == 1)
	{
		printf("Enter the reference number>\n");
		scanf("%d", &delete_ref_key);

	}
	if(deletemenuchoice == 2)
	{

		dat_printsearchoptions();
		/*scanf("%d", &searchchoice);*/
		dat_searchjournals( head, no_journals);
		printf("Enter the reference number or press esc to cancel>\n");
		scanf("%d", &delete_ref_key);

	}

	return delete_ref_key;
}

/*******************************************************************************
 * This function will allow admin users to delete journals from the database
 * The user will have to input the hournal name, authors name, year published
 * and any keywords. This function will be called in the main.
 * inputs: 
 * none
 * outputs: 
 * struct of the journal info. The elements within the struct will be the words
 * which the user will be able to search for journals by.
 * Author: Riza Tolentino
*******************************************************************************/
int dat_delete_Journal( dat_journal_t **head, int key, int no_journals)
{
	dat_journal_t* temp = *head, *prev;

	if(temp!=NULL && temp->referenceno == key)
	{
		*head = temp->next;
		free(temp);
		return no_journals - 1;
	}

	while(temp!=NULL && temp->referenceno != key)
	{
		prev = temp;
		temp = temp->next;
	}

	if(temp == NULL)
	{
		printf("There was an error removing this journal.");
		return no_journals;
	}

	prev->next = temp->next;
	free(temp);
	return no_journals - 1;
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
void dat_print_delete_menu(void)
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

/*******************************************************************************
 * This function will ensure the datatype inputted by its users is correct
 * and lies within the suitable bounds
 * inputs: 
 * searchword - date
 * outputs: 
 * return TRTUE if valid, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_checksearchdate(int date, int month, int year)
{

if(month < 1 || month > 12 || date < 1 || date > 31 ||
     year < 0001 || year > 2018)
    {
        return FALSE;
    }

    return TRUE;


}

/*******************************************************************************
 * This function will ensure the datatype inputted by its users is correct
 * inputs: 
 * searchword from the user - Author name/  journal title or filename
 * outputs: 
 * return TRUE if valid input, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_checkword(char word[])
{	int length;
	int i;
	int invalid = 0;
	length = strlen(word);

	if(length > 256)
	{
		return FALSE;
	}

	for(i=0; i<length; i++)
	{
		if(((word[i]<65 && word[i] != 32) || ((word[i]>90) && (word[i]<97)) || word[i]>122))
		{
			invalid ++;
		}
		
		if(invalid>0)
		{
			return FALSE;

		}	
	}

	return TRUE;


}

/*******************************************************************************
 * This function checks the menu input from the user
 * If the menu input is <lowest option or >highest option, it will inform the
 * user of an "Invalid choice"
 * inputs: 
 * user input from the printed menu list
 * outputs:
 * integer -- TRUE(1)/FALSE (0) to indicate whether the program should continue
*******************************************************************************/
int dat_check_menu_input(int menuinput, int lowerbound, int higherbound)
{

    if(menuinput<lowerbound || menuinput >higherbound)
    {
        printf("Invalid choice\n");
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
 * This function checks the menu input from the user
 * If the menu input is <lowest option or >highest option, it will inform the
 * user of an "Invalid choice"
 * inputs: 
 * user input from the printed menu list
 * outputs:
 * integer -- TRUE(1)/FALSE (0) to indicate whether the program should continue
*******************************************************************************/
int dat_save_journal_data(dat_journal_t *head, int no_journals)
{
	FILE* journalinfo;
	dat_journal_t * current = head->next;
	journalinfo = fopen(DAT_JOURNAL_DB_NAME, "w");


	if(journalinfo)
	{
		fprintf(journalinfo, "%d\n", no_journals);
		while(current != NULL)
			{
			
				fprintf(journalinfo, "%-8.5d %-9.8s %-10.10s %02d %02d %04d \n", current->referenceno, 
				current->journaltitle, current->authoralias,
			 	current->dat_date_dt.date, current->dat_date_dt.month, 
			 	current->dat_date_dt.year);

		 		current = current->next;

			}
	}
	else
	{
		printf("Error writing journal data.\n");
	}
	fclose(journalinfo);
	return 0;
}

/*******************************************************************************
 * This function checks the menu input from the user
 * If the menu input is <lowest option or >highest option, it will inform the
 * user of an "Invalid choice"
 * inputs: 
 * user input from the printed menu list
 * outputs:
 * integer -- TRUE(1)/FALSE (0) to indicate whether the program should continue
*******************************************************************************/
int dat_load_journal_data(dat_journal_t *head)
{
	FILE* journalinfo;
	journalinfo = fopen(DAT_JOURNAL_DB_NAME, "r");
	int no_journals;
	dat_journal_t * current = head->next;

	if(journalinfo == NULL)
	{
		printf("Read error\n");
		return 0;
	}
	if(journalinfo)
	{
		fscanf(journalinfo, "%d", &no_journals);
		while(current != NULL)
			{
			
				fscanf(journalinfo, "%d %s %s %02d %02d %04d \n", &current->referenceno, 
				current->journaltitle, current->authoralias,
			 	&current->dat_date_dt.date, &current->dat_date_dt.month, 
			 	&current->dat_date_dt.year);

		 		current = current->next;

			}
			fclose(journalinfo);
	}
	return no_journals;
}  