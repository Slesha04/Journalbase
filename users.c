#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* by miles */
int hashpassword(char* password)
{
    int hash = 0xDEADBEEF;
    int key[6] = { 0x1C0DE, 0x2DEAD, 0x3C0DE, 0x4ADDD, 0x5BEEF };
    size_t s, len = strlen(password);

    for (s = 0; s < len; s++)
    {
        hash ^= (password[s] * key[s % 6] * 0xC);
    }

    return hash;
}

usr_priv_t usr_login(char* username, char* password)
{

}

int usr_add(char* username, char* password, usr_priv_t privilege)
{
    /*int userid;

    usr_user_t* newuser = (usr_user_t*)malloc(sizeof(usr_user_t));

    newuser->prev = lastinlist;
    newuser->next = 0;

    lastinlist->next = newuser;

    lastinlist = newuser;

    newuser->password_hash = hashpassword(password);

    sscanf(username, "%d", &userid);

    newuser->user_id = userid;
    newuser->privilege = privilege;*/
}

int usr_remove(char* username)
{

}

int user_changepass(char* username, char* oldpassword, char* newpassword)
{

}
