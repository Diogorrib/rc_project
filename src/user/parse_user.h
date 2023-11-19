#ifndef __PARSE_USER_H__
#define __PARSE_USER_H__

/** If login input is not correct return is -1
 */
int confirm_login_input(char *buffer, char *uid, char *pass);

/** Verify if the input is only a command and new line character,
 * if input is not correct return is -1
 */
int confirm_only_cmd_input(char *buffer, const char *cmd);

/** This function return the fname given in the input,
 * if input is not correct return is NULL
 */
char *confirm_open_input(char *buffer, char *name, int *start_value, int *timeactive);

/** Verify if an auction (AID + 1 space + state + (1 space or newline character))
 * is correctly written (if so return 0, else return 1)
 */
int confirm_list(char *msg, char *auction, int index);

/** Verify if the aid from msg is correctly written
 * (if so return 1, else return 0)
 */
int confirm_open(char *msg);


#endif // __PARSE_USER_H__
