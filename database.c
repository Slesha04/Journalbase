/*******************************************************************************
 * database.c: Contains functions for adding to, removing from, searching and
 * reading from the document database.
 * 
 * A linked list of journal structures with a dummy head is maintained and
 * searched.
 *
 * Authors: Riza Tolentino, Miles Burchell
*******************************************************************************/

#include <stdio.h> /* printf, sprintf, scanf, sscanf, fopen, ftell, close, 
                      fread, fwrite, fclose, remove, getchar */
#include <stdlib.h> /* malloc, calloc, free, atoi */
#include <string.h> /* strcpy, strcat, strcmp, strlen */

#include "database.h"
#include "compression.h"
#include "encryption.h"
#include "colours.h"

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
        red();
        printf("Error: dat_add: couldn't open file %s.\n", filename);
        normal(); /*return colour*/
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
        red();
        printf("Error: dat_add: out of memory.\n");
        normal(); /*return colour*/

        fclose(file_stream);

        free(file.data);

        return 1;
    }

    /* read whole file */
    bytes_read = fread(file.data, sizeof(char), file.length, file_stream);

    fclose(file_stream);

    if (bytes_read < file.length)
    {   
        red();
        printf("Error: dat_add: error reading file %s.\n", filename);
        normal();

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
        red();
        printf("Error: dat_add: couldn't open file %s.\n", storename);
        normal(); /*return colour*/

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
        red();
        printf("Error: dat_add: error writing file %s.\n", filename);
        normal(); /*return colour*/

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
        red();
        printf("Error: dat_open: couldn't open file %s.\n", storename);
        normal(); /*return colour*/
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
        red();
        printf("Error: dat_open: out of memory.\n");
        normal(); /*return colour*/

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
        red();
        printf("Error: dat_open: error reading file %s.\n", storename);
        normal(); /*return colour*/

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
        red();
        printf("Error: dat_open: couldn't open temp file.\n");
        normal(); /*return colour*/

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
 *  returns pointer to the newly created journal entry (dat_journal_t), or NULL
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

    int valid = TRUE; /*Indication of valid input*/
    int i = 0, n=0, m=0; /*counters for chars & strings/flags*/
    char  temp, author_buffer; /*buffer for author*/
    char keyword_buffer, temp2; /*buffer for keyword*/

    if (!j)
    {   
        red();/*Change text colour*/
        printf("Error: dat_journalentry: Out of memory.\n");
        normal(); /*return colour*/

        return NULL;
    }

    if (*lastref)
    {
        (*lastref)++;
    }
    
    else
    {
        /*first reference*/
        *lastref = 10001;
    }

    (*j).referenceno = *lastref;

    /*********Start inputting journal information*******************************/
    printf("\nEnter the Journal information>\n");

    /*Prompt user to enter the file they would like to upload*/
    do {
        printf("Enter the File Name>\n");
        scanf(" %[^\n]s", (*j).filename);

        /*This will be the compressed and encrypted file*/
        sprintf((*j).stored_filename, "%d.jb", (*j).referenceno);

#ifdef DEBUG
        printf("DEBUG: The stored filename is: %s\n", (*j).stored_filename);
#endif

        if(dat_add((*j).filename, (*j).stored_filename))
        {
            /*Unsuccessful*/
            valid = FALSE;
        }
        else
        {
            /*Successful*/
            valid = TRUE;
        }

    } while (valid==FALSE);/*until a valid input occurs*/

    do
    {
        printf("\nEnter the Journal Title, no special characters>\n");
        scanf(" %[^\n]s", (*j).journaltitle);

        if (dat_check_word((*j).journaltitle)==FALSE)
        {   
            red();
            printf("Error: Invalid Title. Please try again.\n");
            normal(); /*return colour*/
#ifdef DEBUG
            printf("DEBUG:Error: The title must only"
                    "have alphabet characters.");
#endif
        }

    } while (dat_check_word((*j).journaltitle)==FALSE); /*Ensures 
                                                no special chars.*/

    getchar();/*accepts \n from scanf*/

    do
    {
    	(*j).numberofauthors = 0;
        i = 0;
        n = 0;

        printf("\nEnter the Author's Name, if multiple authors,"
        " separate by a comma and space>\n");
        printf("If the author is unknown, enter *\n");

        while(1)
        {
            valid = TRUE;
            author_buffer = getchar();
            n++;

            if(author_buffer== '\n')
            { 
                /*If the author is unknown, no.authors == 0 and
                the loop breaks*/
                if(temp == '*' && (*j).numberofauthors==0)
                {   
                    i = -1;
                    strcpy((*j).authoralias, "Unknown Author");
                    break;
                }

                /*must enter at least 1 author or "*" */
                /*32 is space in ascii*/
                else if((*j).numberofauthors==0 && ((n==1)||temp == 32))
                {   
                    red();
                    printf("Error: Enter 1 author, if unknown enter '*'\n");
                    normal(); /*return colour*/
                    valid = FALSE;
                }
                /*reset counters at entry of a new line*/
                (*j).authorname[i][n-1] = 0;

                break;
            }


            if(author_buffer != ' ' && author_buffer != ',')
            {
                (*j).authorname[i][n-1] = author_buffer;

            }

            /*checks if listing*/
            if(author_buffer == ' ' && (temp == ','))
            {
                (*j).authorname[i][n-1] = 0;
                n = 0;
                i ++; /*next author begins*/
            }


            temp = author_buffer;

        }

        (*j).numberofauthors = i + 1; /* since i begins at 0*/

#ifdef DEBUG
        printf("Number of authors is: %d\n", (*j).numberofauthors);
#endif
        if((*j).numberofauthors>=1)
        {
            strcpy((*j).authoralias, (*j).authorname[0]);
        }
        if((*j).numberofauthors>1)
        {
            /*if there is more than 1 author, the first author, et al.
            will be displayed*/
            strcat((*j).authoralias, " et al.");
        }

        if((*j).numberofauthors > MAX_NUMBER_AUTHORS)
        {
            valid = FALSE;
            red();
            printf("Error: You have entered too many authors\n");
            normal();
        }

    } while (valid!=TRUE);

    /*Enters the publication date*/
    while(1)
    {

        (*j).dat_date_dt = dat_scan_date();
        if(dat_check_search_date((*j).dat_date_dt)==TRUE)
        {
            /* The user can move on if the date is correct.*/
            break;
        }
        else
        {   

#ifdef DEBUG
            printf("Error: The date does not"
                   " lie within the correct bounds\n");
#endif
        }
    }

#ifdef DEBUG
    char keyword_list[BUFFER_LENGTH+1];
#endif

    /*User inputs keywords or presses enter for none.*/
    do
    {
        i = 0; /*keyword counter*/
        n = 0; /*Char counter for each word*/

        printf("\nThe maximum number of keywords is 5"
            ", press enter for none.\n");
        printf("Enter keywords, separated by a space>\n");

        while(1)
        {
        	/*indicates if the user has inputted too many words*/
            valid = TRUE; 
            keyword_buffer = getchar();
            n++; /*next char*/
            m++; /*counter for keywords list*/

#ifdef DEBUG
        /*compiles the whole list of keywords*/
        keyword_list[m-1]= keyword_buffer;

        if(keyword_buffer=='\n')
        {
            keyword_list[m-1] = 0;
        }
        
#endif

            if (keyword_buffer == 10)
            {
            	/*Null terminate the last word*/
                (*j).journalkeywords[i][n-1] = 0;
                break;

            }

            if(keyword_buffer != ' ' && keyword_buffer != ',')
            {
                (*j).journalkeywords[i][n-1] = keyword_buffer;
            }

            if (keyword_buffer == ' ' && (temp2 == ','))
            {
            	/*null terminate*/
                (*j).journalkeywords[i][n-1] = 0; 
                n = 0; /*back to first letter*/
                i++; /*next word*/
            }


            temp2 = keyword_buffer; /*allows for checking previous char*/

        }

        (*j).numberofkeywords = i + 1;

#ifdef DEBUG
        printf("Number of keywords is: %d\n", (*j).numberofkeywords);
#endif

        if((*j).numberofkeywords > MAX_NUMBER_KEYWORDS)
        {
            valid = FALSE;
            red();
            printf("You have entered too many keywords.\n");
            normal(); /*return colour*/
        }

    } while(valid!=TRUE);

    /*The user will see their reference number in blue*/
    blue();
    printf("\nYour reference number is: %d\n", (*j).referenceno);
    normal();

#ifdef DEBUG
    printf("\nDEBUG: dat_journalentry: journal added with fields:\n"
           " Title: %s\n Author(s): %s\n Publication_date: %d/%d/%d\n"
           " Keywords: %s\n Reference Number: %d\n",
           (*j).journaltitle, (*j).authoralias, (*j).dat_date_dt.date,
           (*j).dat_date_dt.month, (*j).dat_date_dt.year, keyword_list,
           (*j).referenceno);
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
int dat_searchtitle(const char search_term[], const dat_journal_t *head)
{
    int i = 0; /*flag for found search term*/
    dat_journal_t * current = head->next; /*starting point*/
    dat_journal_t * current_printer = head->next; /*starting point*/

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
        printf("\nRef. no. Title           Author          Date Published\n");
        printf("-------- --------------- --------------- --------------\n");
    }

    while(current_printer!=NULL)
    {
        if(strcmp(search_term, current_printer->journaltitle) == 0)
        {
            printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", 
            	current_printer->referenceno,
                   current_printer->journaltitle, 
                   current_printer->authoralias,
                   current_printer->dat_date_dt.date, 
                   current_printer->dat_date_dt.month,
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
int dat_searchauthor(const char search_term[], const dat_journal_t *head)
{
    int i = 0;
    int m = 0; /*node counter*/
    dat_journal_t * current = head -> next;
    dat_journal_t * current_printer = head->next;

    while(current!=NULL)
    {
        for(m=0; m < current->numberofauthors; m++)
        {
#ifdef DEBUG
            printf("DEBUG: The search term is: %s and the author is: %s\n"
                   , search_term, current->authorname[m]);
#endif

            if(strcmp(search_term, current->authorname[m]) == 0)
            {

                i++;
            }
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
        printf("\nRef. no. Title           Author          Date Published\n");
        printf("-------- --------------- --------------- --------------\n");
    }

    m = 0;

    while(current_printer!=NULL)
    {
        for(m=0; m < current_printer->numberofauthors; m++)
        {
            if(strcmp(search_term, current_printer->authorname[m]) == 0)
            {
                printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", 
                	current_printer->referenceno,
                       current_printer->journaltitle, 
                       current_printer->authoralias,
                       current_printer->dat_date_dt.date, 
                       current_printer->dat_date_dt.month,
                       current_printer->dat_date_dt.year);
            }

        }
        current_printer = current_printer->next;
    }

    return TRUE;
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
int dat_searchtags(const char searchkeyword[], const dat_journal_t *head)
{
    int i = 0;
    int m = 0; /*node counter*/
    dat_journal_t * current = head -> next;
    dat_journal_t * current_printer = head->next;

    while(current!=NULL)
    {
        for(m=0; m < current->numberofkeywords; m++)
        {
#ifdef DEBUG
            printf("DEBUG: The search term is: %s and the keywords are: %s\n"
                   , searchkeyword, current->journalkeywords[m]);
#endif

            if(strcmp(searchkeyword, current->journalkeywords[m]) == 0)
            {

                i++;
            }
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

    m = 0;

    while(current_printer!=NULL)
    {
        for(m=0; m < current_printer->numberofkeywords; m++)
        {
            if(strcmp(searchkeyword, current_printer->journalkeywords[m]) == 0)
            {
                printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", 
                		current_printer->referenceno,
                       current_printer->journaltitle, 
                       current_printer->authoralias,
                       current_printer->dat_date_dt.date, 
                       current_printer->dat_date_dt.month,
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
int dat_searchdate(const dat_date_t search_date_term, const dat_journal_t *head)
{
    int i;
    dat_journal_t * current = head->next;
    dat_journal_t * current_printer = head->next;

    while(current!=NULL)
    {
        if(search_date_term.date==current->dat_date_dt.date && 
        	search_date_term.month == current->dat_date_dt.month
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
        if(search_date_term.date==current_printer->dat_date_dt.date && 
        	search_date_term.month == current_printer->dat_date_dt.month
                && search_date_term.year == current_printer->dat_date_dt.year)
        {

            printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", 
            	current_printer->referenceno,
                   current_printer->journaltitle, 
                   current_printer->authoralias,
                   current_printer->dat_date_dt.date, 
                   current_printer->dat_date_dt.month,
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
int dat_search_all(int no_journals,  const dat_journal_t *head)
{
    dat_journal_t * current = head->next;


    printf("Ref. no. Title           Author          Date Published\n");
    printf("-------- --------------- --------------- --------------\n");

    while(current != NULL)
    {

        printf("%-8.5d %-15.14s %-15.15s %02d %02d %04d \n", 
        	current->referenceno,
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
int dat_search_journals(const dat_journal_t *head, int no_journals)
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

        while (1)
        {
        	/*print the menu for journal search*/
            dat_print_search_options();

            scanf("%s", choice_buffer);
            int search_choice = atoi(choice_buffer);

            /*Check for a valid menu*/
            if(dat_check_menu_input(search_choice, 1, 5)==FALSE)
            {
                continue;
            }

            /*Search by Title*/
            if(search_choice == 1)
            {
                printf("\nEnter the Title you are searching for>\n");

                scanf(" %[^\n]s", searchtitle);
                search_success = dat_searchtitle(searchtitle, head);
                return search_success;

            }
            /*Search by Author*/
            if(search_choice == 2)
            {
                printf("\nEnter an Author>\n");
                scanf(" %[^\n]s", searchauthor);
                search_success = dat_searchauthor(searchauthor, head);
                return search_success;
            }
            /*Search by Date*/
            if(search_choice == 3)
            {
            	getchar();
                do
                {
                    searchdate = dat_scan_date();

                }while(dat_check_search_date(searchdate)==FALSE); 
                                            

                search_success = dat_searchdate(searchdate, head);
                return search_success;

            }
             /*Search by Keyword*/
            if(search_choice == 4)
            {
                printf("\nEnter a Keyword>\n");
                scanf(" %[^\n]s", searchkeyword);
                search_success = dat_searchtags(searchkeyword, head);
                return search_success;
            }
            /*Search all journals*/
            if(search_choice == 5)
            {
                search_success = dat_search_all(no_journals, head);
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
int dat_delete_sort(int deletemenuchoice, const dat_journal_t *head, 
                                                            int no_journals)
{

    int delete_ref_key = 0;
    char buffer[BUFFER_LENGTH];
    char validation[] = "Y";
    char validation2[] = "y";


    if(dat_check_menu_input(deletemenuchoice,1,2)==TRUE)
    {
        if(deletemenuchoice == 1)
        {
            printf("Enter the reference number>\n");
            scanf("%d", &delete_ref_key);
        }

        if(deletemenuchoice == 2)
        {
            if(dat_search_journals(head, no_journals))
            {
                printf("Enter the reference number or enter 0 to cancel>\n");
                scanf("%d", &delete_ref_key);

            }
            else
            {
                return 0;
            }
        }

        printf("Journal %d will be deleted. ", delete_ref_key);
        printf("Are you sure you would like to continue?\n"
        	"Press Y to confirm or any key to cancel.\n");
        scanf(" %[^\n]s", buffer);
        if(strcmp(buffer, validation)==0)
        {
            return delete_ref_key;
        }
        else if(strcmp(buffer, validation2)==0)
        {
            return delete_ref_key;
        }
        else
        {
            printf("Deletion aborted.\n");
            return 0;
        }
    }

    return 0;

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
    /*the head node*/
    dat_journal_t* temp = *head, *prev;
	char buffer[BUFFER_LENGTH + 1];

    /*If the node matches the head*/
    if (temp && temp->referenceno == key)
    {
        *head = temp->next; /*move head*/
        free(temp); /*delete matching node*/
        return no_journals - 1; /*one less journal*/
    }

    /*cycles through list until key is reached*/
    while (temp && temp->referenceno != key)
    {
        prev = temp;
        temp = temp->next;
    }

    if (!temp)
    {
        red();
        printf("Error: dat_delete_journal: Couldn't remove journal.\n");
        normal();
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

	/* remove stored file from disk */
	sprintf(buffer, "%d.jb", key);
	remove(buffer);

    printf("Journal %d was deleted from the database.\n", key);

    return no_journals - 1;
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
    printf("2. Search through the journals \n\n");
}

/*******************************************************************************
 * dat_print_search_options
 * This function will print the search options
 * inputs:
 *  none
 * outputs:
 *  none
 * Author: Riza Tolentino
*******************************************************************************/
void dat_print_search_options(void)
{
    printf("\n1. Search by Title\n");
    printf("2. Search by Author\n");
    printf("3. Search by Publication Date\n");
    printf("4. Search by Keywords\n");
    printf("5. Show entire database\n\n");
}

/*******************************************************************************
 * dat_check_search_date
 * This function will ensure the datatype inputted by its users is correct
 * and lies within the suitable bounds
 * inputs:
 *  date, month, year
 * outputs:
 *  return TRUE if valid, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_check_search_date(const dat_date_t pub_date)
{
    if(pub_date.month < MONTH_MIN || pub_date.month > MONTH_MAX ||
            pub_date.date < DAY_MIN || pub_date.date > DAY_MAX ||
            pub_date.year < YEAR_MIN || pub_date.year > YEAR_MAX)
    {
        red();
        printf("Invalid date\n");
        normal();
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
 * dat_check_word
 * This function will ensure the datatype inputted by its users is correct
 * inputs:
 *  searchword from the user - Author name / journal title or filename
 * outputs:
 *  return TRUE if valid input, FALSE if not
 * Author: Riza Tolentino
*******************************************************************************/
int dat_check_word(const char word[])
{   int length;
    int i;
    int invalid = 0;
    length = strlen(word);

    if(length > 256)
    {
        return FALSE;
    }

    for(i=0; i<length; i++)
    {
    	/*ASCII chars*/
        if(((word[i]<65 && word[i] != 32) || ((word[i]>90) && (word[i]<97)) 
        	|| word[i]>122))
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
        red();
        printf("Invalid selection. Please enter a number between");
        printf(" %d and %d.\n", lowerbound, higherbound);
        normal();
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
    dat_journal_t* journal;

    fp = fopen(DAT_JOURNAL_DB_NAME, "w+");

    if (!fp)
    {
        red();
        printf("Error: dat_save_journal_data: Couldn't open file.\n");
        normal();
        return 1;
    }

    /* write no of journals */
    fwrite(&no_journals, sizeof(int), 1, fp);

    journal = head -> next;

    /* write journal data */
    while(journal)
    {
        fwrite(journal, sizeof(dat_journal_t), 1, fp);
        journal = journal->next;
    }

    fclose(fp);

    return 0;
}

/*******************************************************************************
 * dat_load_journal_data
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
    int valid = TRUE; /*Checks if valid input*/
    int i, m; /*counters for the chars*/
    char whole_date[256]; /*array for the whole date*/
    char date_buffer; 
    dat_date_t date;

    do
    {
        valid = TRUE;

        printf("\nEnter the publication date in the format DD/MM/YYYY>\n");

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

            /* Numeric input for date */
            if (i == 0 || i == 1 || i == 3 || i == 4 || (i >= 6 && i<= 9))
            {
                /*checks if numeral*/
                if (date_buffer < '0' || date_buffer > '9')
                {
                    valid = FALSE;
                }

                whole_date[i]=date_buffer;
            }

            /* Separators '/'*/
            if (i == 2 || i == 5)
            {
                if (date_buffer != '/')
                {
                    valid = FALSE;
                }

                whole_date[i] = 32;
            }
            /*Check length of date input*/
            if(i > 9)
            {
                valid = FALSE;
            }
        }
        if(i!=9)
        {
            valid = FALSE;
        }

        /*error was detected while scanning*/
        if(valid == FALSE)
        {   
            red();
            printf("Invalid input\n");
            normal();
        }
    } while(valid==FALSE); /*repeat until a valid date*/

    whole_date[m] = '\0'; /*Null terminated*/

    sscanf(whole_date, "%d%d%d", &date.date, &date.month, &date.year);

    return date;
}
