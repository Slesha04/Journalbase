#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct login
{
    char first_name[20];
    char last_name[20];
    char username[10];
    char password[10];
};
typedef struct login login_t;
void main_menu(void);
void admin_display(void);
void user_display(void);
void register_member_user(void);
void login_members(void);
void register_admin_user(void);
void login_admins(void);
int main()
{
    main_menu();
    
    
}


void main_menu()
{
    int choice = 0 ;
    printf("\n\nWelcome to JournalBase \nThe Safest Journal Storage!!\n\n");
    printf("Please select the type of user you are to log in\n");
    printf("1. Admin\n2. Member/Regular User\n\n");
    scanf("%d",&choice);
    
    if(choice==1)
    {
        admin_display();
        
    }
    else if (choice ==2)
    {
        user_display();

    }else if (choice == 3)
    {
        exit(0);
        
    }
        
    else{
        printf("Invalid Input\n");
        main_menu();
    }
}

void admin_display()
{
    int choice = 0 ;
    int count_admin =0;
    
    printf("Please select an Action\n");
    printf("1. Login\n2. Register\n3. Exit\n4.Go Back\n");
    scanf("%d",&choice);
    
    
    if(choice==1)
    {
        login_admins();
        
    }
    else if (choice ==2)
    {
        register_admin_user();
        count_admin++;
        
    }else if(choice == 3)
    {
        exit(0);
    }else if(choice == 4 )
    {
        main_menu();
    }
    else {
        printf("Invalid Input");
        admin_display();
    }
}

void user_display()
{
    int choice = 0 ;
    int count_admin =0;
    printf("Please select an Action\n");
      printf("1. Login\n2. Register\n3. Exit\n4.Go Back\n");
    scanf("%d",&choice);
    if(choice==1)
    {
        login_members();
    }
    else if (choice ==2)
    {
        register_member_user();
        count_admin++;
        
    }else if(choice == 3)
    {
        exit(0);
    }else if(choice == 4 )
    {
        main_menu();
    }
    else {
        printf("Invalid Input");
        user_display();
    }
}

void register_member_user()
{
    FILE * login_member;
    login_member = fopen("login_member","w+");
    login_t user_login;
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
    login_members();
}

void login_members()
{
    char username[20];
    char password[20];
    FILE * login_member;
    login_member = fopen("login_member","r");
    login_t user_login;
    
    printf("Username : ");
    scanf("%s", username);
    
    printf("Password : ");
    scanf("%s", password);
    
    
    while(fread(&user_login,sizeof(user_login),1,login_member))
    {
        if (strcmp(username,user_login.username)==0 && strcmp(password,user_login.password)==0)
        {
            printf("Successful Login\n");
        }else{
            printf("\nPlease Enter Correct Username and Password\n");
            login_members();
        }
    }
    
    fclose(login_member);
}

void register_admin_user()
{
    FILE * login_admin;
    login_admin = fopen("login_admin","w");
    login_t user_login;
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
    login_admins();
}

void login_admins()
{
    char username[20];
    char password[20];
    FILE * login_admin;
    login_admin = fopen("login_admin","r");
    login_t user_login;
    
    printf("Username : ");
    scanf("%s", username);
    
    printf("Password : ");
    scanf("%s", password);
    
    
    while(fread(&user_login,sizeof(user_login),1,login_admin))
    {
        if (strcmp(username,user_login.username)==0 && strcmp(password,user_login.password)==0)
        {
            printf("Successful Login\n");
        }else{
            printf("\nPlease Enter Correct Username and Password\n");
            login_admins();
        }
    }
    fclose(login_admin);
}
