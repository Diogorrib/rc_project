#ifndef __PARSE_USER_H__
#define __PARSE_USER_H__

int confirm_login_input(char *buffer, char *uid, char *pass);

int confirm_only_cmd_input(char *buffer, const char *cmd);

char *confirm_open_input(char *buffer, char *name, int *start_value, int *timeactive);

#endif // __PARSE_USER_H__
