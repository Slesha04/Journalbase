/*******************************************************************************
 * database.c: Contains functions for adding to, removing from, searching and
 * reading from the document database.
 * 
 * Authors: Riza Tolentino, Miles Burchell
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "compression.h"
#include "encryption.h"

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
	printf("DEBUG: dat_add: adding file %s!\n", filename);
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

		free(file.data);

        return 1;
    }

	/* read whole file */
	bytes_read = fread(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_add: error reading file %s.\n", filename);

		free(file.data);

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

		free(file.data);

        return 1;
    }

	/* write store file real length (uint32), followed by data */
	bytes_read = fwrite(&file.real_length, sizeof(unsigned int), 1, 
		file_stream);
	bytes_read = fwrite(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	free(file.data);

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
	int retn_val = 0;

	file_stream = fopen(storename, "r"); /*open file stream */

	if (!file_stream)
    {
        printf("Error: dat_open: couldn't open file %s.\n", storename);
        return 1;
    }

	/* get length */
	fseek(file_stream, 0 , SEEK_END);
	/* file length minus real_length int at start of file */
    file.length = ftell(file_stream) - sizeof(unsigned int);
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

		free(file.data);

        return 1;
    }

	/* read real length */
	bytes_read = fread(&file.real_length, sizeof(unsigned int), 1,
		 file_stream);
	/* read all data */
	bytes_read = fread(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	if (bytes_read < file.length)
    {
        printf("Error: dat_open: error reading file %s.\n", storename);

		free(file.data);

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

		free(file.data);

        return 1;
    }

	/* write temp file */
	bytes_read = fwrite(file.data, sizeof(char), file.length, file_stream);

	fclose(file_stream);

	free(file.data);

	if (bytes_read < file.length)
    {
        printf("Error: dat_open: error writing to temp file.\n");

		return 1;
    }

	/* attempt to open temp file in gvim */

	retn_val = system("gvim temp.txt");

	#ifdef DEBUG
	printf("DEBUG: Failed to open file in gvim. Using vim instead.\n");
	#endif

	/* otherwise use vim */
	if (retn_val != 0)
	{
		printf("\n\nSince GVim is not available in your envrionment, the ");
		printf("article will be opened in this terminal window. Type ':qa!'");
		printf(" when you are ready to exit the article viewer.\n\n");
		printf("Press enter to continue...\n");

		getchar();
		getchar();

		system("vim temp.txt");
	}

	return 0;
}

/*******************************************************************************
 * dat_journalentry
 * This function will prompt users to enter the file name of the journal they 
 * would like to add and then prompt the user for filename, journal title, 
 * author(s), date, keyword(s)
 * inputs: 
 *  no_journals: number of journals
 *  lastref: last assigned journal reference number (read/write)
 * outputs: 
 *  returns pointer to the newly create journal entry (dat_journal_t), or NULL
 *  if failed.
 * Author: Riza Tolentino
*******************************************************************************/
dat_journal_t *dat_journalentry(int no_journals, int* lastref)
{
	#ifdef DEBUG
	printf("DEBUG: dat_journalentry: adding journal no. %d.\n", no_journals);
	#endif

	dat_journal_t *j = NULL;
	j = malloc(sizeof(dat_journal_t));

	int valid = TRUE;
	int i = 0, m = 0, k=0, n=0, x=0; /*counters/flags*/
	char keyword_buffer, temp, author_buffer;
	
    if (!j)
	{	
	    printf("ERROR: dat_journalentry: Out of memory.\n");

		return NULL;
	}

	if (*lastref)
	{
		(*lastref)++;
	}
	else
	{
		*lastref = 10001;
	}

	(*j).referenceno = *lastref;

	printf("Enter the Journal information>\n");

	do{
		printf("Enter the File Name>\n");
		scanf(" %[^\n]s", (*j).filename);

		sprintf((*j).stored_filename, "%d.jb", (*j).referenceno);

		 #ifdef DEBUG
		 printf("DEBUG: The stored filename is: %s\n", (*j).stored_filename);
		 #endif

		if(dat_add((*j).filename, (*j).stored_filename))
		{
			valid = FALSE;
		}

		else
		{
			valid = TRUE;
		}
	} while (valid==FALSE);

	do
	{
		printf("Enter the Journal Title>\n");
		scanf(" %[^\n]s", (*j).journaltitle);

		if (dat_checkword((*j).journaltitle)==FALSE)
		{
			printf("Invalid Title. Please try again.\n");
		}

	} while (dat_checkword((*j).journaltitle)==FALSE);

	do
	{
		printf("Enter the Author's Name, if multiple authors, separate by");
		printf(" a comma and space>\n");
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
			
			temp = author_buffer;

		}while(author_buffer != 10);

		(*j).numberofauthors = n + 1;
		if((*j).numberofauthors>1)
		{
			strcat((*j).authoralias, "et al.");
		}

		if((*j).numberofauthors>MAX_NUMBER_AUTHORS)
		{
			valid = FALSE;
			printf("You have entered too many authors\n");
		}
	} while (valid!=TRUE);
	
	while(1)
	{
	/*if the input has been previously invalid but skipped the first warning, 
	the user will now be warned*/
		valid = TRUE;
		(*j).dat_date_dt = dat_scan_date();
		if(dat_checksearchdate((*j).dat_date_dt.date,(*j).dat_date_dt.month,(*j).dat_date_dt.year)==TRUE)
		{
			break;
		}
		else
		{
			printf("Invalid date.\n");
		}
	}
		
	#ifdef DEBUG
	char keyword_list[MAX_KEYWORD_LENGTH+1];
	#endif

	do
	{	
		i = 0;
		valid = TRUE;

		printf("The maximum number of keywords is 5\n");
		printf("Enter keywords, separated by a space>\n");
		
		do
		{
			if (keyword_buffer == 10)
			{
				i = 0; /*word count*/
				m = 0; /*letter count*/
				n = 0;
			}

			keyword_buffer = getchar();

			m++;
			n++;

			#ifdef DEBUG
			keyword_list[n-1]=keyword_buffer;
			#endif

			if(keyword_buffer != ' ' && keyword_buffer != ',')
			{
				(*j).journalkeywords[m-1][i] = keyword_buffer;
			}

			if (keyword_buffer == ' ' && (temp == ',' || m == 0))
			{
				(*j).journalkeywords[m-1][i] = 0;
				m=0;
				i++;
			}

			#ifdef DEBUG
			if(keyword_buffer==10)
			{
				keyword_list[n-1] = 0;
			}
			#endif
			
			temp = keyword_buffer;
		}while(keyword_buffer != 10);

		(*j).numberofkeywords = i + 1;
		#ifdef DEBUG
		printf("Number of keywords is: %d\n", (*j).numberofkeywords);
		#endif
		
		if((*j).numberofkeywords>MAX_NUMBER_KEYWORDS)
		{
			valid = FALSE;
			printf("You have entered too many keywords.\n");
		}

	} while (!valid);

	printf("Your reference number is: %d\n", (*j).referenceno);

	#ifdef DEBUG
	printf("DEBUG: dat_journalentry: journal added with fields:\nTitle: %s\n Author(s): %s\n Publication_date: %d/%d/%d\n Keywords: %s\n Reference Number: %d\n", 
			(*j).journaltitle, (*j).authoralias, (*j).dat_date_dt.date, 
			(*j).dat_date_dt.month, (*j).dat_date_dt.year, keyword_list,(*j).referenceno);
	#endif

return j;

}

/*******************************************************************************
 * This function will compare the search term to titles within the linked list
 * inputs: 
 * search term (title being searched), head of the linked list.
 * outputs: 
 * printed list of journals with the relevant tile. 
 * returns TRUE if a match was found for the journal being searched, returns 
 * FALSE if no match was found
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
 * This function will compare the searched author to authors within the linked 
 * list
 * inputs: 
 * search term (author being searched), head of the linked list.
 * outputs: 
 * printed list of journals with the relevant author. 
 * returns TRUE if a match was found for the journal being searched, returns 
 * FALSE if no match was found
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
		
	if (s == 0)
	{
		printf("No matches were found\n");
	}

	if (s > 0)
	{
		printf("Ref. no. Title           Author          Date Published\n");
		printf("-------- --------------- --------------- --------------\n");		
	}

	while (current_printer)
	{
		no_authors = current_printer->numberofkeywords;

		for (n = 0; n<no_authors; n++, i = 0)
		{
			for (j = 0; j<length0; j++)
			{
				if (search_term[j] == current_printer->authorname[j][n])
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
 * This function will compare the searched keyword to titles within the 
 * linked list
 * inputs: 
 * search term (keyword being searched), head of the linked list.
 * outputs: 
 * printed list of journals with the keyword. 
 * returns TRUE if a match was found for the journal being searched, returns 
 * FALSE if no match was found
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
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");			
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
 * This function will compare the search date to publication dates
 * within the linked list
 * inputs: 
 * search term (publication date being searched), head of the linked list.
 * outputs: 
 * printed list of journals with the publication date. 
 * returns TRUE if a match was found for the journal being searched, returns 
 * FALSE if no match was found
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
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");		
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
 * This function will print all the journal info for all journals in the linked
 * list
 * inputs: 
 * number of journals, head of the linked list
 * outputs: 
 * printed list of all journals. Will return 1 for a successful print.
 * Author: Riza Tolentino
*******************************************************************************/	
int dat_searchall(int no_journals, dat_journal_t *head)
{
	dat_journal_t * current = head->next;
		
 
			printf("Ref. no. Title           Author          Date Published\n");
			printf("-------- --------------- --------------- --------------\n");		
			
			while(current != NULL)
			{
			
				printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", current->referenceno, 
				current->journaltitle, current->authoralias,
			 	current->dat_date_dt.date, current->dat_date_dt.month, 
			 	current->dat_date_dt.year);

		 		current = current->next;

			}
		
		return TRUE;
}

/*******************************************************************************
 * This function allows users to search through the journals based on fields.
 * inputs: 
 * pointer to the head of the linked list, number of journals in the list.
 * outputs: 
 * returns TRUE if a match was found for the journal being searched, returns 
 * FALSE if no match was found
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

	if (no_journals == 0)
	{
		printf("There are no journals to search through.\n");

		return 0;
	}
	else
	{
		dat_printsearchoptions();

		while (1)
		{
			scanf("%s", choice_buffer);
			int search_choice = atoi(choice_buffer);
		
			if(dat_check_menu_input(search_choice, 1, 5)==FALSE)
			{
				continue;
			}

			if(search_choice == 1)
			{
				printf("Enter the Title you are searching for>\n");

				scanf(" %[^\n]s", searchtitle);
				search_success = dat_searchtitle(searchtitle, head);
				return search_success;

			}
			if(search_choice == 2)
			{
				printf("Enter an Author>\n");
				scanf(" %[^\n]s", searchauthor);
				search_success = dat_searchauthor(searchauthor, head);
				return search_success;
			}
			if(search_choice == 3)
			{
			
				/*printf("Enter the Publication Date of the Journal>\n");*/
				searchdate = dat_scan_date();
				search_success = dat_searchdate(searchdate, head);
				return search_success;

			}
			if(search_choice == 4)
			{
				printf("Enter a Keyword>\n");
				scanf(" %[^\n]s", searchkeyword);
				search_success = dat_searchtags(searchkeyword, head);
				return search_success;
			}
			if(search_choice == 5)
			{
				search_success = dat_searchall(no_journals, head);
				return search_success;
			}	
		}
	}
	
}

/*******************************************************************************
 * This function will allow admin users to sort through journals before
 * decided which journal to delete from the database
 * The user will can input the journal name, authors name, date published
 * and any keywords. This function will be called in the main.
 * inputs: 
 * delete menu choice, head of the linked list, number of journals
 * outputs: 
 * the reference number of the journal they would like to delete. If 0 is 
 * returned, no journal is to be deleted
 * Author: Riza Tolentino
*******************************************************************************/
int dat_delete_sort(int deletemenuchoice, dat_journal_t *head, int no_journals)
{

	int delete_ref_key = 0;
	
	
	if(dat_check_menu_input(deletemenuchoice,1,2)==TRUE)
	{
		if(deletemenuchoice == 1)
		{
			printf("Enter the reference number>\n");
			scanf("%d", &delete_ref_key);
		}

		if(deletemenuchoice == 2)
		{
			if(dat_searchjournals(head, no_journals))
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
 * dat_delete_journal
 * This function deleteds the journal & journal info from the linked list of
 * the journal that is to be deleted - as identified with the reference no.
 * inputs: 
 *  head: head of the linked list
 *  key: reference number of the journal to be deleted
 *  no_journals: number of journals.
 * outputs: 
 *  Returns the new number of journals in the linked list. If a journal is 
 *  deleted, there will be 1 less journal in the list.
 * Author: Riza Tolentino
*******************************************************************************/
int dat_delete_journal (dat_journal_t **head, int key, int no_journals)
{
	dat_journal_t* temp = *head, *prev;

	if (temp && temp->referenceno == key)
	{
		*head = temp->next;
		free(temp);
		return no_journals - 1;
	}

	while (temp && temp->referenceno != key)
	{
		prev = temp;
		temp = temp->next;
	}

	if (!temp)
	{
		printf("ERROR: dat_delete_journal: Couldn't remove journal.\n");
		return no_journals;
	}

	if (temp->next)
	{
		prev->next = temp->next;
		temp->next->prev = prev;
	}
	else
	{
		prev->next = 0;
	}
	
	free(temp);
	
	printf("Journal %d was deleted from the database.\n", key);

	return no_journals - 1;
}

/*******************************************************************************
 * dat_print_menu
 * This function will print database options
 * inputs: 
 *  none
 * outputs: 
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void dat_print_menu(void)
{
	printf("\n1. Add journal to the database\n");
	printf("2. Delete journal from the database\n");
	printf("3. Search for a journal\n");
	printf("4. Open a journal\n");
	printf("5. Go back to main menu\n");
}

/*******************************************************************************
 * dat_print_delete_menu
 * This function will print the database delete options
 * inputs: 
 * 	none
 * outputs: 
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void dat_print_delete_menu(void)
{
	printf("\n1. Enter the reference number of the journal to be deleted\n");
	printf("2. Search through the journals \n");
}

/*******************************************************************************
 * dat_printsearchoptions
 * This function will print the search options
 * inputs: 
 *  none
 * outputs: 
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void dat_printsearchoptions(void)
{
	printf("\n1. Search by Title\n");
	printf("2. Search by Author\n");
	printf("3. Search by Publication Date\n");
	printf("4. Search by Keywords\n");
	printf("5. Search through all\n");
}

/*******************************************************************************
 * dat_checksearchdate
 * This function will ensure the datatype inputted by its users is correct
 * and lies within the suitable bounds
 * inputs: 
 *  date, month, year
 * outputs: 
 *  return TRUE if valid, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_checksearchdate(int date, int month, int year)
{
	if (month < MONTH_MIN || month > MONTH_MAX ||
		date < DAY_MIN || date > DAY_MAX ||
		year < YEAR_MIN || year > YEAR_MAX)
	{
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
 * dat_checkword
 * This function will ensure the datatype inputted by its users is correct
 * inputs: 
 *  searchword from the user - Author name / journal title or filename
 * outputs: 
 *  return TRUE if valid input, FALSE if not
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
 * dat_check_menu_input
 * This function checks the menu input from the user
 * If the menu input is <lowest option or >highest option, it will inform the
 * user.
 * inputs: 
 *  menuinput: user input
 *  lowerbound: lowest valid choice
 *  higherbound: highest valid choice
 * outputs:
 *  return TRUE if menu input is valid, FALSE if it is not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_check_menu_input(int menuinput, int lowerbound, int higherbound)
{
    if(menuinput < lowerbound || menuinput > higherbound)
    {
        printf("Invalid selection. Please enter a number between");
		printf("%d and %d.\n", lowerbound, higherbound);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
 * dat_save_journal_data
 * This function saves the journal database to disk.
 * inputs: 
 * pointer to dat_file_t structure
 * outputs: 
 * return 0 if successful, 1 if failed.
 * Author: Miles Burchell
*******************************************************************************/
int dat_save_journal_data(dat_journal_t* head, int no_journals)
{
	#ifdef DEBUG
	printf("DEBUG: Saving database to disk\n");
	#endif

	FILE* fp;
	int i;
	dat_journal_t* journal;

	fp = fopen(DAT_JOURNAL_DB_NAME, "w+");

	if (!fp)
	{
		printf("Error: dat_save_journal_data: Couldn't open file.\n");

		return 1;
	}

	/* write no of journals */
	fwrite(&no_journals, sizeof(int), 1, fp);

	journal = head;

	/* write journal data */
	for (i = 0; i < no_journals; i++)
	{
		journal = journal->next;

		if (!journal)
		{
			printf("Error: dat_save_journal_data: Null pointer.\n");
			return 1;
		}

		fwrite(journal, sizeof(dat_journal_t), 1, fp);
	}

	fclose(fp);

	return 0;
}

/*******************************************************************************
 * dat_loads_journal_data
 * This function loads the journal database from disk.
 * inputs: 
 *  head: head node of journal list
 *  lastref: pointer to write last assigned reference ID to (read/write)
 * outputs: 
 *  returns number of journals read if successful, or 0 if failed
 * Author: Miles Burchell
*******************************************************************************/
int dat_load_journal_data(dat_journal_t* head, int* lastref)
{
	#ifdef DEBUG
	printf("DEBUG: Loading database from disk\n");
	#endif

	FILE* fp;
	int i, no_journals, max_ref = 0;
	dat_journal_t * journal, * current;

	fp = fopen(DAT_JOURNAL_DB_NAME, "r+");

	if (!fp)
	{
		return 0;
	}

	/* read no of journals */
	fread(&no_journals, sizeof(int), 1, fp);

	current = head;
	current->prev = 0;

	/* read each journal and add it to the list */
	for (i = 0; i < no_journals; i++)
	{
		journal = (dat_journal_t*)calloc(sizeof(dat_journal_t), 1);

		fread(journal, sizeof(dat_journal_t), 1, fp);

		if (journal->referenceno > max_ref)
		{
			max_ref = journal->referenceno;
		}

		journal->prev = current;
		current->next = journal;
		current = journal;
	}

	current->next = 0;

	*lastref = max_ref;

	return no_journals;
}

/*******************************************************************************
 * dat_scan_date
 * This function scans for the date inputted by the user &checks for a valid
 * input.
 * If the menu input is <lowest option or >highest option, it will inform the
 * user of an "Invalid choice"
 * inputs: 
 *  none
 * outputs:
 *  returns user inputted date as dat_date_t
 * Author: Riza Tolentino
*******************************************************************************/
dat_date_t dat_scan_date(void)
{
	int valid = TRUE;
	int i, m;
	char whole_date[25];
	char date_buffer;
	dat_date_t date;

	do
	{
		valid = TRUE;

		printf("Enter the publication date in the format DD/MM/YYYY>\n");

		m = 0;

		while (1)
		{
			date_buffer = (char)getchar();

			if (date_buffer == '\n')
			{
				break;
			}

			m++;
			i=m-1;

			/* Numeric input */
			if (i == 0 || i == 1 || i == 3 || i == 4 || (i >= 6 && i<= 9))
			{

				if (date_buffer < '0' || date_buffer > '9')
				{
					valid = FALSE;
				}

				whole_date[i]=date_buffer;	
			}

			/* Separators */
			if (i == 2 || i == 5)
			{
				if (date_buffer != '/')
				{
					valid = FALSE;
				}

				whole_date[i] = 32;
			}

			if(i > 9)
			{
				valid = FALSE;
			}
		}
		if(i!=9)
		{
			valid = FALSE;
		}

		if(valid == FALSE)
        {
            printf("Invalid input\n");
        }
	}while(valid==FALSE);

	whole_date[m] = '\0';

	sscanf(whole_date, "%d%d%d", &date.date, &date.month, &date.year);

	return date;
}
