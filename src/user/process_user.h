#ifndef __PROCESS_USER_H__
#define __PROCESS_USER_H__

/** Get command and status from received message 
 */
void get_cmd_status(char *msg, char * cmd, char *status);

/** Verify if response message from AS is correct, 
 * if login was made return 1, else return 0
 */
int process_login(char *msg_received, char *uid);

/** Verify if response message from AS is correct, 
 * if logout was made return 1, else return 0
 */
int process_logout(char *msg, const char *uid);

/** Verify if response message from AS is correct, 
 * if logout was made return 1, else return 0
 */
int process_unregister(char *msg, const char *uid);

/** Verify if the aid from msg is correctly written
 * (if so return 1, else return 0)
 */
int confirm_open(char *msg);

/** Verify if response message from AS is correct, 
 * gives feedback to the user whether the operation was executed successfully or not
 */
void process_open(char *msg);

/** Verify if response message from AS is correct, 
 * gives feedback to the user whether the operation was executed successfully or not
 */
void process_close(char *msg, char *aid, const char *uid);

/** Append an auction (AID + 1 space + state + (1 space or newline character))
 * to the string
 */
void append_auction(char *string, char *auction);

/** Transforms the list of auctions received from AS into a list of auctions
 * and their status (active or not active) separated by newline character
 * if return is -1 there is an auction that is not correctly written
 */
int get_auctions(char *destination, char *msg);

/** Verify if an auction (AID + 1 space + state + (1 space or newline character))
 * is correctly written (if so return 0, else return 1)
 */
int confirm_list(char *msg, char *auction, int index);

/** Verify if response message from AS is correct, 
 * print in each line auction AID and if that auction is active or not
 */
void process_list(char *msg);

/** Verify if response message from AS is correct, 
 * print in each line auction AID and if that auction is active or not
 */
void process_ma(char *msg);

/** Verify if response message from AS is correct, 
 * print in each line auction AID and if that auction is active or not
 */
void process_mb(char *msg);

int confirm_bid(char *bid);

int build_bid(char *bid, int offset, char *msg, char *uid, int *bid_value, char *bid_datetime, int *bid_sec_time);
int get_bids_list(char *bids, char *msg, int offset);

int get_bids(char *bids, char *msg, int offset);

void process_sr(char *msg, char *aid);

#endif // __PROCESS_USER_H__
