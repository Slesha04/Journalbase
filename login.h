#ifndef LOG_H
#define LOG_H

#include "types.h"

void log_main_menu(void);
void log_admin_display(void);
void log_user_display(void);
void log_register_member_user(void);
void log_login_members(void);
void log_register_admin_user(void);
void log_login_admins(void);
void log_scan_password(char password[]);

#endif
