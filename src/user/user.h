#ifndef __USER_H__
#define __USER_H__

#include "../common/constant.h"
#include "../common/utils.h"

/** Verify the parameters from the start of User app, if present -p and -n
 * update IP and PORT to use, if not present default parameters are used
 */
void filter_input(int argc, char **argv);

/** Send buffer to AS and receive msg_received from AS using UDP,
 * if return is -1 the connection to AS went wrong
 */
int udp(char *buffer, size_t size, char *msg_received);

/** Send buffer to AS and receive msg_received from AS using TCP,
 * if return is -1 the connection to AS went wrong.
 * This function can also send or receive files if fname != NULL
 */
int tcp(char *msg_sent, char *fname, ssize_t size, char *msg_received);

/** Return 1 if there is no uid or password else return 0
*/
int no_uid_pass(char *command);

/** Save the login data (uid and password) and set logged_in to 1
 */
void user_login(const char *aux_uid, const char *aux_pass);

/* The following functions are used to carry out the operations given by the commands referred to in the project statement */

void login();
void logout();
void unregister();
void open_auction();
void close_auction();
void myauctions(char *first_word);
void mybids(char *first_word);
void list(char *first_word);
void show_asset(char *first_word);
void bid(char *first_word);
void show_record(char *first_word);

#endif // __USER_H__
