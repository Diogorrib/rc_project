#ifndef __PROCESS_USER_H__
#define __PROCESS_USER_H__

/// @brief  Get command and status from received message 
/// @param msg 
/// @param cmd 
/// @param status 
void get_cmd_status(char *msg, char * cmd, char *status);

/// @brief Verify if response message from AS is correct, 
/// if login was made return 1, else return 0.
/// Gives feedback to the user whether the operation was executed successfully or not
/// @param msg_received 
/// @param uid 
int process_login(char *msg_received, char *uid);

/// @brief Verify if response message from AS is correct, 
/// if logout was made return 1, else return 0.
/// Gives feedback to the user whether the operation was executed successfully or not
/// @param msg 
/// @param uid 
int process_logout(char *msg, const char *uid);

/// @brief Verify if response message from AS is correct, 
/// if logout was made return 1, else return 0.
/// Gives feedback to the user whether the operation was executed successfully or not
/// @param msg 
/// @param uid 
/// @return 
int process_unregister(char *msg, const char *uid);

/// @brief Verify if the aid from msg is correctly written. If so return 1, else return 0
/// @param msg 
/// @return 
int confirm_open(char *msg);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg 
void process_open(char *msg);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg 
/// @param aid 
/// @param uid 
void process_close(char *msg, char *aid, const char *uid);

/// @brief Append an auction, AID and its state (active or not active) to the end of the string
/// @param string 
/// @param auction 
void append_auction(char *string, char *auction);

/// @brief Transforms the list of auctions received from AS into a list of auctions
/// and their state (active or not active) separated by newline character
/// if return is -1 there is an auction that is not correctly written
/// @param destination 
/// @param msg 
/// @return 
int get_auctions(char *destination, char *msg);

/// @brief Verify if an auction (AID + 1 space + state + (1 space or newline character))
/// is correctly written (if so return 0, else return 1)
/// @param msg 
/// @param auction 
/// @param index 
/// @return 
int confirm_list(char *msg, char *auction, int index);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg 
void process_list(char *msg);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg 
void process_ma(char *msg);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg 
void process_mb(char *msg);

/// @brief Read fname and fsize during tcp connection. If something wrong appens return -1
/// @param fd 
/// @param fname 
/// @param fsize 
/// @return 
int get_fname_fsize(int fd, char *fname, long *fsize);

/// @brief Verify if response message from AS is correct, print file's name and size.
/// Also performs the download of the file
/// @param fd 
/// @param fname 
/// @param msg 
/// @return 
int process_sa(int fd, char *fname, char *msg);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg 
/// @param aid 
void process_bid(char *msg, char *aid);

/// @brief Verify if a bid (started with B and ending in space or newline character)
/// is correctly written, if so return the offset for the next bid in the message,
/// else function return 0
/// @param msg 
/// @param initial 
/// @param uid 
/// @param value 
/// @param date 
/// @param bid_time 
/// @return 
long confirm_bid(char *msg, long initial, char *uid, long *value, char *date, int *bid_time);

/// @brief  Transforms the list of bids received from AS into a list of bids
/// (with the format "uid: x" + tab + "bid_value: y" + tab + "time_of_bid: z" +
/// "time_since_start: w seconds") separated by newline character
/// if return is 0 there is a bid that is not correctly written,
/// else return the offset for the E part of the message (if this part exists)
/// @param bids 
/// @param msg 
/// @param offset 
/// @return 
long get_bids_list(char *bids, char *msg, long offset);

/// @brief Verify if response message from AS is correct, 
/// create the message to be shown to user:
///
/// Bids from auction aid - [nome da auction ;  (file name)] hosted by UID started with value y, at date_time. Will be open during x seconds:\n
/// [uid: %s bid_value: %s   time_of_bid: YYYY-MM-DD hh:mm:ss    time_since_start: x seconds\n]
/// Ended at date_time, opened for x seconds.\n  
///
/// @param bids 
/// @param msg 
/// @param initial 
/// @return 
int get_bids(char *bids, char *msg, int initial);

/// @brief Verify if response message from AS is correct, print all the info about the asset aid
/// @param msg 
/// @param aid 
void process_sr(char *msg, char *aid);


#endif // __PROCESS_USER_H__
