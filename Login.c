/*******************************************************************************
 * login.c: Contains functions for user authentication and management.
 * 
 * Authors: Slesha Mishra
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "login.h"
#include "database.h"

void log_main_menu()
{
    int choice = 0 ;
    printf("\n\nWelcome to JournalBase \nThe Safest Journal Storage!!\n\n");
    printf("Please select the type of user you are to log in\n");
    printf("1. Admin\n2. Member/Regular User\n\n");
    scanf("%d",&choice);
    
    if(choice==1)
    {
        log_admin_display();
        
    }
    else if (choice ==2)
    {
        log_user_display();

    }else if (choice == 3)
    {
        exit(0);
        
    }
        
    else{
        printf("Invalid Input\n");
        log_main_menu();
    }
}

void log_admin_display()
{
    int choice = 0 ;
    int count_admin = 0;
    
    printf("Please select an Action\n");
    printf("1. Login\n2. Register\n3. Exit\n4.Go Back\n");
    scanf("%d",&choice);
    
    
    if(choice==1)
    {
        log_login_admins();
        
    }
    else if (choice ==2)
    {
        log_register_admin_user();
        count_admin++;
        
    }else if(choice == 3)
    {
        exit(0);
    }else if(choice == 4 )
    {
        log_main_menu();
    }
    else {
        printf("Invalid Input");
        log_admin_display();
    }
}

void log_user_display()
{
    int choice = 0 ;
    int count_admin =0;
    printf("Please select an Action\n");
      printf("1. Login\n2. Register\n3. Exit\n4.Go Back\n");
    scanf("%d",&choice);
    if(choice==1)
    {
        log_login_members();
    }
    else if (choice ==2)
    {
        log_register_member_user();
        count_admin++;
        
    }else if(choice == 3)
    {
        exit(0);
    }else if(choice == 4 )
    {
        log_main_menu();
    }
    else {
        printf("Invalid Input");
        log_user_display();
    }
}

void log_register_member_user()
{
    FILE * login_member;
    login_member = fopen("login_member","r+");
    log_login_t user_login;
    printf("\nYour First Name\n");
    scanf("%s",user_login.first_name);
    
    printf("Your Last Name\n");
    scanf("%s",user_login.last_name);
    
    printf("Please Set a Username\n");
    scanf("%s",user_login.username);
    
    printf("Please set a password\n");
    scanf("%s",user_login.password);
    
    fwrite(&user_login,sizeof(user_login),1,login_member);
    fclose(login_member);
    printf("Now that you've registered , Please Login \n");
    log_login_members();
}

void log_login_members()
{
    char username[20];
    char password[20];
    FILE * login_member;
    login_member = fopen("login_member","r+");
    log_login_t user_login;
    
    printf("Username : ");
    scanf("%s", username);
    
    printf("Password : ");
    system("stty -echo");
    scanf("%s", password);
    system("stty echo");
   

    
    while(fread(&user_login,sizeof(user_login),1,login_member))
    {
        if (strcmp(username,user_login.username)==0 && strcmp(password,user_login.password)==0)
        {
            printf("\nSuccessful Login\n");
            
            fclose(login_member);
            return;
        }
    }
    
    printf("\nPlease Enter Correct Username and Password\n");
    log_login_members();
    
    fclose(login_member);
}

void log_register_admin_user()
{
    FILE * login_admin;
    login_admin = fopen("login_admin","r+");
    log_login_t user_login;
    printf("\nEnter First Name\n");
    scanf("%s",user_login.first_name);
    
    printf("Enter Last Name\n");
    scanf("%s",user_login.last_name);
    
    printf("Please Set a Username\n");
    scanf("%s",user_login.username);
    
    printf("Please set a password\n");
    scanf("%s",user_login.password);
    
    fwrite(&user_login,sizeof(user_login),1,login_admin);
    fclose(login_admin);
    printf("Now that you've registered , Please Login \n");
    log_login_admins();
}

void log_login_admins()
{
    char username[20];
    char password[20];
    FILE * login_admin;
    login_admin = fopen("login_admin","r+");
    log_login_t user_login;
    
    printf("Username : ");
    scanf("%s", username);
    
    printf("Password : ");
    system("stty -echo");
    scanf("%s", password);
    system("stty echo");
    
    
    while(fread(&user_login,sizeof(user_login),1,login_admin))
    {
        if (strcmp(username,user_login.username)==0 && strcmp(password,user_login.password)==0)
        {
            printf("Successful Login\n");
        }else{
            printf("\nPlease Enter Correct Username and Password\n");
            log_login_admins();
        }
    }
    fclose(login_admin);
}
void log_scan_password(char password[])

{
    system("stty -echo");
    scanf("%s", password);
    system("stty echo");
    printf("%s\n", password);
}


