#ifndef __USER_H__
#define __USER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/* struct Login {
    char uid[7];
    int logged_in = 0;
}; */

/** Verify the parameters from the start of user app, if present -p and -n
 * update ip and port to use, if not present default parameters are used
 */
void filter_input(int argc, char **argv);

/** Send buffer to AS and receive msg_received from AS using UDP,~
 * if return is -1 the connection to AS went wrong
*/
int udp(char *buffer, size_t size, char *msg_received);

/** Save the login data (uid and password) and set logged_in to 1
*/
void user_login(const char **login_data);

void login();

void logout();

void unregister();

#endif // __USER_H__