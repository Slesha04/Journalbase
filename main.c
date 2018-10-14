#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"
#include "encryption.h"
#include "database.h"

int main(void)
{

	/*

    FILE* f = fopen("comp_test.txt", "r");

    if (!f)
    {
        printf("Couldn't open comp_test.txt\n");
        return 1;
    }


    dat_file_t testfile;

    fseek (f, 0 , SEEK_END);
    testfile.length = ftell(f);
    rewind(f);
    
    testfile.compressed = 0;
    testfile.encrypted = 0;
    testfile.data = calloc(sizeof(char), testfile.length);

    fread(testfile.data, sizeof(char), testfile.length, f);

    printf("Read %d bytes\n", testfile.length);

    fclose(f);

    com_compressfile(&testfile);


  */

 /*database testing*/
    int dat_menu_selection=0; 
    char choice_buffer[2];
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
			relative_count = dat_delete_Journal(&head, delete_key, relative_count);
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

	}while(dat_menu_selection != 4);

    /*f = fopen("compressed.txt", "w+");
    fwrite(testfile.data, sizeof(char), testfile.length, f);
    fclose(f);

    com_decompressfile(&testfile);

    f = fopen("decompressed.txt", "w+");
    fwrite(testfile.data, sizeof(char), testfile.length, f);
    fclose(f);


    free(testfile.data);*/

    /*printf("Data has been compressed and decompressed: %s\n", 
        (char*)testfile.data);*/


    return 0;



}