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
 * if input is not correct return is -1
 */
int confirm_open_input(char *buffer, char *name, char *fname, int *start_value, int *timeactive);

/** If command plus aid input is not correct return is -1
 */
int confirm_aid_input(char *buffer, char *cmd, char *aid);

/** If command plus aid plus the bid_value input is not correct return is -1
 */
int confirm_bid_input(char *buffer, char *cmd, char *aid, char *bid_value);

#endif // __PARSE_USER_H__
