#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

/////////////////////////////////////////// READING FILES /////////////////////////////////////////////////////////////

/// @brief Read the password (8 characters) from a (uid)_pass.txt file
/// @param fname should be a (uid)_pass.txt file
/// @param existing_pass Buffer that is going to receive the existing password in the (uid)_pass.txt file
/// @return -1 in case of error
int read_password_file(const char *fname, char *existing_pass);

/// @brief Read all fields from a START_(aid).txt file
/// @param path should be a START_(aid).txt file
/// @param uid Buffer that is going to receive the existing UID in the START_(aid).txt file
/// @param name Buffer that is going to receive the existing description name of the auction in the START_(aid).txt file
/// @param fname Buffer that is going to receive the existing fname of the auction in the START_(aid).txt file
/// @param start_value Integer that is going to receive the existing start value of the auction in the START_(aid).txt file
/// @param timeactive Integer that is going to receive the existing time that the auction will be active in the START_(aid).txt file
/// @param datetime Buffer that is going to receive the existing password in the START_(aid).txt file
/// @param starttime Long integer that is going to receive the existing time since the 
/// 1970-01-01 untill the auction was created in seconds in the START_(aid).txt file
/// @return -1 in case of error
int read_start_file(const char *path, char *uid, char *name, char *fname, int *start_value,
                    int *timeactive, char *datetime, long *starttime);

/// @brief Get needed fields from START_(aid).txt file to the show record command
/// @param fname_start should be a START_(aid).txt file
/// @param msg Buffer that is going to receive the needed fields
/// @return -1 in case of error
int read_start(const char *fname_start, char *msg);

/// @brief Get needed fields from END_(aid).txt file to the show record command
/// @param fname_end should be a END_(aid).txt file
/// @param msg Buffer that is going to receive the needed fields
/// @return -1 in case of error
int read_end(const char *fname_end, char *msg);

/////////////////////////////////////////// VERIFY AUCTIONS EXPIRED ///////////////////////////////////////////////////

/// @brief End all auctions that expired
void verify_all_end();

/// @brief If auction aid is expired end it
/// @param aid Auction ID
/// @return -1 if auction aid does not exist
int verify_auction_end(const char *aid);

////////////////////////////////////// GET LISTS AND HIGHEST VALUES ///////////////////////////////////////////////////

/// @brief Scan HOSTED or BIDDED directory and append to msg all AIDs in that directory,
/// and their state (active or not active), sorted by AID ascending.
/// If there is no file the msg content is NOK
/// @param dirname should be HOSTED or BIDDED directory
/// @param cmd Buffer that stores the instruction that we are making (MA or MB)
/// @param msg Buffer that is going to receive the message to send to the user
void get_auctions(const char *dirname, const char *cmd, char *msg);

/// @brief Scan BIDS directory and append to msg the 50 highest bids in that directory,
/// and relevant content to show record command, sorted by bid value descending.
/// If there is no file the content appended to the msg is empty
/// @param dirname should be BIDS directory
/// @param msg Buffer that is going to receive the message to send to the user
void get_bids(const char *dirname, char *msg);

/// @brief Scan BIDS directory and get to bid_value the value of the highest bid in that directory,
/// if there is no file the highest bid is the value present in start_file
/// @param dirname should be BIDS directory
/// @param start_file should be a START_(aid).txt file
/// @param bid_value Buffer that is going to receive the highest bid there is in the BIDS directory to send to the user
void get_highest_bid(const char *dirname, const char *start_file, char *bid_value);

/////////////////////////////////////////// UDP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if login was made, 0 otherwise
int process_login(const char *uid, const char *pass, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if logout was made, 0 otherwise
int process_logout(const char *uid, const char *pass, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if unregister was made, 0 otherwise
int process_unregister(const char *uid, const char *pass, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if the list of the user's auctions was sent, 0 otherwise
int process_ma(const char *uid, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
//// @param uid Buffer that stores the UID received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if the list of the user's bids was sent, 0 otherwise
int process_mb(const char *uid, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param msg Buffer that is going to receive the message to send to the user 
/// @return 1 if the list of the auctions was sent, 0 otherwise
int process_list(char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the AID received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if all the information about the auction was sent, 0 otherwise
int process_sr(const char *aid, char *msg);

/////////////////////////////////////////// TCP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param name Buffer that stores the description name received from the user
/// @param start_value Buffer that stores the start value of the auction received from the user
/// @param timeactive Buffer that stores the time that the auction will be active received from the user
/// @param fname Buffer that stores the fname of the auction received from the user
/// @param aid Buffer that stores the AID (Auction ID) created by the server
/// @param buffer Buffer that is going to receive the message to send to the user
/// @return 1 if the auction was created, 0 otherwise
int process_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *aid, char *buffer);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param aid Buffer that stores the AID received from the user
/// @param buffer Buffer that is going to receive the message to send to the user
/// @return 1 if the auction was close, 0 otherwise
int process_close(const char *uid, const char *pass, const char *aid, char *buffer);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param aid Buffer that stores the AID received from the user
/// @param fname Buffer that stores the fname of the auction received from the user
/// @param fsize Buffer that is going to receive the size of the file to send to the user 
/// @param msg Buffer that is going to receive the message to send to the user
/// @return 1 if the asset was shown to the user, 0 otherwise
int process_sa(const char *aid, char *fname, long *fsize, char *msg);

/// @brief Verify if it is possible to execute the operation requested by the user, 
/// gives feedback to the user whether the operation was executed successfully or not
/// @param uid Buffer that stores the UID received from the user
/// @param pass Buffer that stores the password received from the user
/// @param aid Buffer that stores the AID received from the user
/// @param bid_value Buffer that stores the bid value made from the user
/// @param buffer Buffer that is going to receive the message to send to the user
/// @return 1 if the bid was made, 0 otherwise
int process_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer);


#endif // __PROCESS_SERVER_H__
