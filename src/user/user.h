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
int tcp(char *buffer, ssize_t size, char *msg_received);

/** Save the login data (uid and password) and set logged_in to 1
*/
void user_login(const char *aux_uid, const char *aux_pass);

/** Use udp function to receive a message with LOGIN_MSG char
 * and get from that message the command and status
*/
int get_cmd_status(char *buffer, char *msg, char * cmd, char *status);

void login();

void logout();

void unregister();

/** Verify if an auction (AID + 1 space + state + (1 space or newline character))
 * is correctly written (if so return 0, else return 1)
*/
int confirm_list(char *msg, char *auction, int i);

/** Append an auction (AID + 1 space + state + (1 space or newline character))
 * to the string
*/
void append_auction(char *string, char *auction);

/** Transforms the list of auctions received from AS into a list of auctions
 * and their status (active or not active) separated by newline character
 * if return is -1 there is an auction that is not correctly written
*/
int get_list(char *destination, char *msg);

void list(char *first_word);

void open();

#endif // __USER_H__
