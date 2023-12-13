#ifndef __PROCESS_USER_H__
#define __PROCESS_USER_H__

/////////////////////////////////// HELPER FUNCTIONS TO PROCESS COMMANDS //////////////////////////////////////////////

/// @brief  Get command and status from received message 
/// @param msg Buffer that stores the message received from the server
/// @param cmd Buffer that is going to receive the command response from the server
/// @param status Buffer that is going to receive the status from the server
void get_cmd_status(char *msg, char * cmd, char *status);

/// @brief Verify if the aid from msg is correctly written
/// @param msg Buffer that stores the message received from the server
/// @return 1 if msg is correctly written, 0 otherwise
int confirm_open(char *msg);

/// @brief Append an auction, AID and its state (active or not active) to the end of the string
/// @param string Buffer that is going to receive the next auction and add it to the already constructed 
/// string or to an empty string if the auction is the first one that it is being analysed
/// @param auction Buffer that stores an auction received from the server
void append_auction(char *string, char *auction);

/// @brief Transforms the list of auctions received from AS into a list of auctions
/// and their state (active or not active) separated by newline character
/// @param msg Buffer that stores the message received from the server
/// @param destination Buffer that is going to receive all the auctions that the user contains
/// @return -1 if there is an auction that is not correctly written, 0 otherwise
int get_auctions(char *msg, char *destination);

/// @brief Verify if an auction (AID + 1 space + state + (1 space or newline character)) is correctly written
/// @param msg Buffer that stores the message received from the server
/// @param auction Buffer that stores a specific auction received from the server
/// @param index Integer that indicates which auction is being analysed
/// @return 0 if the auction is correctly written, 1 otherwise
int confirm_list(char *msg, char *auction, int index);

/// @brief Read fname and fsize during tcp connection
/// @param fd Buffer that stores the file descriptor received from the server
/// @param fname Buffer that is going to receive the name of the auction given from the server
/// @param fsize Integer that indicates the file size
/// @return -1 if something wrong appens return 
int get_fname_fsize(int fd, char *fname, long *fsize);

/// @brief Verify if a bid (started with B and ending in space or newline character) is correctly written
/// @param msg Buffer that stores the message received from the server
/// @param initial Integer that indicates the initial position of the bid that is being analysed
/// @param uid Buffer that is going to receive the UID from the server for this specific bid
/// @param value Integer that is going to receive the bid value from the server for this specific bid
/// @param date Variable that is going to receive the date time from the server for this specific bid
/// @param bid_time Integer that is going to receive the time in seconds since the beginnig of the 
/// auction from the server for this specific bid
/// @return offset for the next bid in the message if a bid is correctly written, 0 otherwise
long confirm_bid(char *msg, long initial, char *uid, long *value, char *date, int *bid_time);

/// @brief  Transforms the list of bids received from AS into a list of bids
/// (with the format "uid: x" + tab + "bid_value: y" + tab + "time_of_bid: z" +
/// "time_since_start: w seconds") separated by newline character
/// if return is 0 there is a bid that is not correctly written,
/// else return the offset for the E part of the message (if this part exists)
/// @param msg Buffer that stores the message received from the server
/// @param bids Buffer that is going to store the string of bids to show to the user
/// @param offset Integer to walk through the whole message received from the server
/// @return 0 if there is a bid that is not correctly written,
/// else return the offset for the E part of the message (if this part exists)
long get_bids_list(char *msg, char *bids, long offset);

/// @brief Verify if response message from AS is correct, 
/// create the message to be shown to user:
///
/// Bids from auction aid - [nome da auction ;  (file name)] hosted by UID started with value y, at date_time. Will be open during x seconds:\n
/// [uid: %s bid_value: %s   time_of_bid: YYYY-MM-DD hh:mm:ss    time_since_start: x seconds\n]*
/// Ended at date_time, opened for x seconds.\n  
///
/// @param msg Buffer that stores the message received from the server
/// @param bids Buffer that is going to store the all string to show to the user for the command 
/// show record except the following initial part:
/// Bids from auction aid - 
/// @param initial Integer that indicates the initial position of the message received from the server that is being analysed
/// @note The param initial is needed because the initial offset is not zero, it is CMD_N_SPACE+STATUS-1.
/// @return 0 if the message received is correctly written, -1 otherwise
int get_bids(char *msg, char *bids, int initial);

///////////////////////////////// FUNCTIONS THAT PROCESS A SPECIFIC COMMAND ///////////////////////////////////////////

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
/// @param uid Buffer that stores the UID received from the server
/// @return 1 if login was made, 0 otherwise
int process_login(char *msg, char *uid);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
/// @param uid Buffer that stores the UID already received from the user
/// @return 1 if logout was made, 0 otherwise
int process_logout(char *msg, const char *uid);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
/// @param uid Buffer that stores the UID already received from the user
/// @return 1 if logout was made, 0 otherwise
int process_unregister(char *msg, const char *uid);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
void process_open(char *msg);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
/// @param aid Buffer that stores the AID received from the user
/// @param uid Buffer that stores the UID already received from the user
void process_close(char *msg, char *aid, const char *uid);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg Buffer that stores the message received from the server
void process_list(char *msg);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg Buffer that stores the message received from the server
void process_ma(char *msg);

/// @brief Verify if response message from AS is correct, 
/// print in each line auction AID and if that auction is active or not
/// @param msg Buffer that stores the message received from the server
void process_mb(char *msg);

/// @brief Verify if response message from AS is correct, print file's name and size.
/// Also performs the download of the file
/// @param msg Buffer that stores the message received from the server
/// @param fd Buffer that stores the file descriptor of the filname received from the server
/// @param fname Buffer that is going to receive the name of the auction given from the server
/// @return 0 if show asset was made successfully, -1 otherwise
int process_sa(char *msg, int fd, char *fname);

/// @brief Verify if response message from AS is correct, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that stores the message received from the server
/// @param aid Buffer that stores the AID received from the user
void process_bid(char *msg, char *aid);

/// @brief Verify if response message from AS is correct, print all the info about the asset aid
/// @param msg Buffer that stores the message received from the server
/// @param aid Buffer that stores the AID received from the user
void process_sr(char *msg, char *aid);


#endif // __PROCESS_USER_H__
