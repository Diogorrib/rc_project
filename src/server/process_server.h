#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

/////////////////////////////////////////// READING FILES /////////////////////////////////////////////////////////////

/// @brief Read the password (8 characters) from a (uid)_pass.txt file
/// @param fname should be a (uid)_pass.txt file
/// @param existing_pass 
/// @return -1 in case of error
int read_password_file(const char *fname, char *existing_pass);

/// @brief Read all fields from a START_(aid).txt file
/// @param path should be a START_(aid).txt file
/// @param uid 
/// @param name 
/// @param fname 
/// @param start_value 
/// @param timeactive 
/// @param datetime 
/// @param starttime 
/// @return -1 in case of error
int read_start_file(const char *path, char *uid, char *name, char *fname, int *start_value,
                    int *timeactive, char *datetime, long *starttime);

/// @brief Get needed fields from START_(aid).txt file to the show record command
/// @param fname_start should be a START_(aid).txt file
/// @param msg 
/// @return -1 in case of error
int read_start(const char *fname_start, char *msg);

/// @brief Get needed fields from END_(aid).txt file to the show record command
/// @param fname_end should be a END_(aid).txt file
/// @param msg 
/// @return 
int read_end(const char *fname_end, char *msg);

/////////////////////////////////////////// VERIFY AUCTIONS EXPIRED ///////////////////////////////////////////////////

/// @brief End all auctions that expired
void verify_all_end();

/// @brief If auction aid is expired end it
/// @param aid 
/// @return -1 if auction aid does not exist
int verify_auction_end(const char *aid);

////////////////////////////////////// GET LISTS AND HIGHEST VALUES ///////////////////////////////////////////////////

/// @brief Scan HOSTED or BIDDED directory and append to msg all AIDs in that directory,
/// and their state (active or not active), sorted by AID ascending.
/// If there is no file the msg content is NOK
/// @param dirname should be HOSTED or BIDDED directory
/// @param cmd 
/// @param msg 
void get_auctions(const char *dirname, const char *cmd, char *msg);

/// @brief Scan BIDS directory and append to msg the 50 highest bids in that directory,
/// and relevant content to show record command, sorted by bid value descending.
/// If there is no file the content appended to the msg is empty
/// @param dirname should be BIDS directory
/// @param msg 
void get_bids(const char *dirname, char *msg);

/// @brief Scan BIDS directory and get to bid_value the value of the highest bid in that directory,
/// if there is no file the highest bid is the value present in start_file
/// @param dirname should be BIDS directory
/// @param start_file should be a START_(aid).txt file
/// @param bid_value 
void get_highest_bid(const char *dirname, const char *start_file, char *bid_value);

/////////////////////////////////////////// UDP ///////////////////////////////////////////////////////////////////////
/* The following functions return 1 if the command is performed and 0 if the command is not performed */

int process_login(const char *uid, const char *pass, char *msg);
int process_logout(const char *uid, const char *pass, char *msg);
int process_unregister(const char *uid, const char *pass, char *msg);
int process_ma(const char *uid, char *msg);
int process_mb(const char *uid, char *msg);
int process_list(char *msg);
int process_sr(const char *aid, char *msg);

/////////////////////////////////////////// TCP ///////////////////////////////////////////////////////////////////////
/* The following functions return 1 if the command is performed and 0 if the command is not performed */

int process_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *aid, char *buffer);
int process_close(const char *uid, const char *pass, const char *aid, char *buffer);
int process_sa(const char *aid, char *fname, long *fsize, char *msg);
int process_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer);


#endif // __PROCESS_SERVER_H__
