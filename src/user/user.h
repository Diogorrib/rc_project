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
 * if return is -1 the connection to AS went wrong
 */
int tcp(char *msg_sent, char *fname, ssize_t size, char *msg_received);

/** Return 1 if there is no uid or password else return 0
*/
int no_uid_pass();

/** Save the login data (uid and password) and set logged_in to 1
 */
void user_login(const char *aux_uid, const char *aux_pass);

void login();

void logout();

void unregister();

void list(char *first_word);

void open_auction();

void close_auction();

#endif // __USER_H__
