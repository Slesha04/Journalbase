#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/*******************************************************************************
 * dat_add
 * This function reads a file from disk, compresses it, encrypts it, and writes
 * it to disk with a new name.
 * inputs: 
 *  filename: name of file to open
 *  storename: name of file to write to (will create/open)
 * outputs: 
 *  return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int dat_add(const char* filename, const char* storename)
{
	#ifdef DEBUG
	printf("DEBUG: dat_add: adding file %s.\n", filename);
	#endif

	dat_file_t file; /* file in memory */
	FILE* file_stream;
	size_t bytes_read = 0;

	file_stream = fopen(filename, "r"); /*open file stream */

	if (!file_stream)
    {
        printf("Error: dat_add: couldn't open file %s.\n", filename);
        return 1;
    }

	/* get length */
	fseek(file_stream, 0 , SEEK_END);
    file.length = ftell(file_stream);
    rewind(file_stream);

	/* set flags */
	file.compressed = FALSE;
	file.encrypted = FALSE;
	/* allocate memory for file contents in memory */
	file.data = calloc(sizeof(char), file.length + 1);

	if (!file.data)
    {
        printf("Error: dat_add: out of memory.\n");

		fclose(file_stream);

        return 1;
    }

	/* read whole file */
	bytes_read = fread(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_add: error reading file %s.\n", filename);

        return 1;
    }

	/* compress file */
	com_compressfile(&file);

	/* encrypt file */
	enc_encryptfile(&file);

	/* open store file for writing */
	file_stream = fopen(storename, "w");

	if (!file_stream)
    {
        printf("Error: dat_add: couldn't open file %s.\n", storename);

		fclose(file_stream);

        return 1;
    }

	/* write store file */
	bytes_read = fwrite(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_add: error writing file %s.\n", filename);

        return 1;
    }

	return 0;
}

/*******************************************************************************
 * dat_open
 * This function reads a stored encrypted and compressed file from disk, 
 * decrypts and decomrpesses it and writes it to temp.txt for use. temp.txt
 * must be disposed of after use.
 * inputs: 
 *  storename: name of file to open
 * outputs: 
 *  return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int dat_open(const char* storename)
{
	#ifdef DEBUG
	printf("DEBUG: dat_open: opening file %s.\n", storename);
	#endif

	dat_file_t file; /* file in memory */
	FILE* file_stream;
	size_t bytes_read = 0;

	file_stream = fopen(storename, "r"); /*open file stream */

	if (!file_stream)
    {
        printf("Error: dat_open: couldn't open file %s.\n", storename);
        return 1;
    }

	/* get length */
	fseek(file_stream, 0 , SEEK_END);
    file.length = ftell(file_stream);
    rewind(file_stream);

	/* set flags */
	file.compressed = TRUE;
	file.encrypted = TRUE;
	/* allocate memory for file contents in memory */
	file.data = calloc(sizeof(char), file.length + 1);

	if (!file.data)
    {
        printf("Error: dat_open: out of memory.\n");

		fclose(file_stream);

        return 1;
    }

	/* read whole file */
	bytes_read = fread(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_open: error reading file %s.\n", storename);

        return 1;
    }

	/* decrypt */
	enc_decryptfile(&file);

	/* decompress */
	com_decompressfile(&file);

	/* open temp file for writing */
	file_stream = fopen("temp.txt", "w");

	if (!file_stream)
    {
        printf("Error: dat_open: couldn't open temp file.\n");

		fclose(file_stream);

        return 1;
    }

	/* write temp file */
	bytes_read = fwrite(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_open: error writing to temp file.\n");

        return 1;
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

		if(dat_add((*j).filename, "stored.txt")==FALSE)
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
	dat_date_t searchdate;
	int search_success = TRUE;

	if(no_journals == 0)
	{
		printf("There are no journals to search through\n");
	}

	else{
	dat_printsearchoptions();
	scanf("%s", choice_buffer);


	int search_choice = atoi(choice_buffer);
	/*CHECK MENU INPUT*/
	if(search_choice == 1)
	{
		printf("Enter the Title you are searching for>\n");

		scanf(" %[^\n]s", searchtitle);
		search_success = dat_searchtitle(searchtitle, head);

	}
	if(search_choice == 2)
	{
		printf("Enter an Author>\n");
		scanf(" %[^\n]s", searchauthor);
		search_success = dat_searchauthor(searchauthor, head);
	}
	if(search_choice == 3)
	{
	
		/*printf("Enter the Publication Date of the Journal>\n");*/
		searchdate = dat_scan_date();
		search_success = dat_searchdate(searchdate, head);
		

	}
	if(search_choice == 4)
	{
		printf("Enter a Keyword>\n");
		scanf(" %[^\n]s", searchkeyword);
		search_success = dat_searchtags(searchkeyword, head);

	}
	if(search_choice == 5)
	{
		search_success = dat_searchall(no_journals, head);

	}	
	}
	return search_success;
}

/*******************************************************************************
 * This function will print database options
 * inputs: 
 * -
 * outputs: 
 * printed menu
 * Author: Riza Tolentino
*******************************************************************************/
int dat_searchtitle(char search_term[], dat_journal_t *head)
{
	int i = 0;
	dat_journal_t * current = head->next;
	dat_journal_t * current_printer = head->next;

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
			return FALSE;
		}

		if(i>0)
		{
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");	
		}
	
	
		while(current_printer!=NULL)
		{
			if(strcmp(search_term, current_printer->journaltitle) == 0)
			{

			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current_printer->referenceno, 
			current_printer->journaltitle, current_printer->authoralias,
		 	current_printer->dat_date_dt.date, current_printer->dat_date_dt.month, 
		 	current_printer->dat_date_dt.year);

			}

			current_printer = current_printer->next;
		}
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
int dat_searchauthor(char search_term[], dat_journal_t *head)
{

	int n = 0;
	int i = 0;
	int s  = 0;
	int j = 0;
	int length0;
	int no_authors;

 dat_journal_t * current = head->next;
 dat_journal_t * current_printer = head->next;

    while(current!=NULL)
		{	
			
			length0=strlen(search_term);
			no_authors= current->numberofauthors;

			for(n=0; n<no_authors; n++, i=0)
			{
				for(j=0; j<length0; j++)
				{
					if(search_term[j] == current->authorname[j][n])
					{
						i++;

					}

				}
			
				if(i==length0)
				{
					s++; /*indicates if the length of the entered word matches the keyword*/
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


		while(current_printer!=NULL)
		{
			no_authors = current_printer->numberofkeywords;

			for(n=0; n<no_authors; n++, i=0)
			{
				for(j=0; j<length0; j++)
				{
					if(search_term[j] == current_printer->authorname[j][n])
					{
						i++;	
					}

				}

				if(i == length0)
				{	
					printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current_printer->referenceno, 
					current_printer->journaltitle, current_printer->authoralias,
				 	current_printer->dat_date_dt.date, current_printer->dat_date_dt.month, 
				 	current_printer->dat_date_dt.year);
				}
			}
			
			current_printer = current_printer->next;
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
 dat_journal_t * current_printer = head->next;

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
					s++; /*indicates if the length of the entered word matches the keyword*/
				}
			}

			current = current->next;
		}
		if(s==0)
		{
			printf("No matches were found\n");
			return FALSE;
		}
		if(s>0)
		{
			printf("Ref. no. Title     Author     Date Published\n");
			printf("-------- --------- ---------- --------------\n");		
		}
	
	 /*dat_journal_t * current = head->next;*/

		while(current_printer!=NULL)
		{
			no_keywords = current_printer->numberofkeywords;

			for(n=0; n<no_keywords; n++, i=0)
			{
				for(j=0; j<length1; j++)
				{
					if(searchkeyword[j] == current_printer->journalkeywords[j][n])
					{
						i++;	
					}

				}

				if(i == length1)
				{	
					printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current_printer->referenceno, 
					current_printer->journaltitle, current_printer->authoralias,
				 	current_printer->dat_date_dt.date, current_printer->dat_date_dt.month, 
				 	current_printer->dat_date_dt.year);
				}
			}
			
			current_printer = current_printer->next;
		}

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
int dat_searchdate(dat_date_t search_date_term, dat_journal_t *head)
{
	int i;
 	dat_journal_t * current = head->next;
	dat_journal_t * current_printer = head->next;
	
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
			return FALSE;
		}
		if(i>0)
		{
			printf("Ref. no. Title     Author     Date Published\n");
			printf("-------- --------- ---------- --------------\n");		
		}
	
	 /*dat_journal_t * current = head->next;*/
		while(current_printer!=NULL)
		{
			if(search_date_term.date==current_printer->dat_date_dt.date && search_date_term.month == current_printer->dat_date_dt.month
				&& search_date_term.year == current_printer->dat_date_dt.year)
			{
				
			printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current_printer->referenceno, 
			current_printer->journaltitle, current_printer->authoralias,
		 	current_printer->dat_date_dt.date, current_printer->dat_date_dt.month, 
		 	current_printer->dat_date_dt.year);

			}

			current_printer = current_printer->next;
		}
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
int dat_searchall(int no_journals, dat_journal_t *head)
{
	dat_journal_t * current = head->next;
		
 
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
int dat_delete_sort(int deletemenuchoice, dat_journal_t *head, int no_journals)
{

	int delete_ref_key = 0;
	
	
	if(dat_check_menu_input(deletemenuchoice,1,2)==TRUE)
	{

	if(deletemenuchoice == 1) /*ASCII*/
	{
		printf("Enter the reference number>\n");
		scanf("%d", &delete_ref_key);

	}
	if(deletemenuchoice == 2)
	{

		
		/*scanf("%d", &searchchoice);*/
		if(dat_searchjournals( head, no_journals)==TRUE)
		{
		printf("Enter the reference number or enter 0 to cancel>\n");
		scanf("%d", &delete_ref_key);
		}
		else
		{
			return 0;
		}
	}
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

/*******************************************************************************
 * This function checks the menu input from the user
 * If the menu input is <lowest option or >highest option, it will inform the
 * user of an "Invalid choice"
 * inputs: 
 * user input from the printed menu list
 * outputs:
 * integer -- TRUE(1)/FALSE (0) to indicate whether the program should continue
*******************************************************************************/
dat_date_t dat_scan_date(void)
{
	char d_buffer[BUFFER_LENGTH+1], m_buffer[BUFFER_LENGTH+1], y_buffer[BUFFER_LENGTH+1];
	dat_date_t publication_date;
	int valid = TRUE, validcheck2 = TRUE;
	char check;

	printf("Enter the Publication Date of the Journal>\n");
	scanf(" %s", d_buffer);
		check = getchar();
		if(check==10)
			{
				valid=FALSE;
				validcheck2 = FALSE;
				
			}

		else
			{
				scanf("%s", m_buffer);
				check = getchar();
				if(check==10)
				{
					valid=FALSE;
					validcheck2 = FALSE;
					
				}
				else
				{
					scanf("%s", y_buffer);  
				}

			}

		publication_date.date = atoi(d_buffer);
		publication_date.month = atoi(m_buffer);
		publication_date.year = atoi(y_buffer);

if(valid!=TRUE||validcheck2!=TRUE)
	{
		printf("Invalid date\n");		
	}



return publication_date;
}