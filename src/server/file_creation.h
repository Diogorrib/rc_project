#ifndef __FILE_CREATION_H__
#define __FILE_CREATION_H__

////////////////////////////////////// USERS DIRECTORY FILES //////////////////////////////////////////////////////////

/// @brief Function to create a login file for a user
/// @param uid User ID
/// @return 1 if the file was created, 0 otherwise
int create_login(const char *uid);

/// @brief Function to create a password file for a user
/// @param uid User ID
/// @param pass User password
/// @return 1 if the file was created, 0 otherwise
int create_password(const char *uid, const char *pass);

/// @brief Function to create a hosted file for a user
/// @param uid User ID
/// @param aid Auction ID
/// @return 1 if the file was created, 0 otherwise
int create_hosted(const char *uid, const char *aid);

/// @brief Function to create a bidded file for a user
/// @param uid User ID
/// @param aid Auction ID
/// @return 1 if the file was created, 0 otherwise
int create_bidded(const char *uid, const char *aid);

////////////////////////////////////// AUCTIONS DIRECTORY FILES ///////////////////////////////////////////////////////

/// @brief Function to create a start file for an auction
/// @param aid Auction ID
/// @param fdata Data to be written to the file
/// @return 1 if the file was created, 0 otherwise
int create_start(const char *aid, const char *fdata);

/// @brief Function to create an asset file for an auction
/// @param fd File descripor for communication
/// @param aid Auction ID
/// @param fname Name of the asset file
/// @param fsize Size of the asset file
/// @return 1 if the file was created, 0 otherwise
int create_asset(int fd, const char *aid, const char *fname, const char *fsize);

/// @brief Function to create an end file for an auction
/// @param aid Auction ID
/// @param fdata Data to be written to the file
/// @return 1 if the file was created, 0 otherwise
int create_end(const char *aid, char *fdata);

/// @brief Function to create a bid file for an auction
/// @param aid Auction ID
/// @param bid_value Value of the bid to be the name of the file
/// @param fdata Data to be written to the file
/// @return 1 if the file was created, 0 otherwise
int create_bid_value(const char *aid, int bid_value, char *fdata);

//////////////////////////////////// MULTIPLE FILE CREATION OR CONDITIONAL ////////////////////////////////////////////

/// @brief Get the actual time and create 2 files: one in ...(uid)/HOSTED directory and
/// other in AUCTIONS/(aid) directory. Also write fdata + the actual time to this second file
/// @param aid Auction ID
/// @param uid User ID
/// @param fdata Data to be written to the file
/// @return 1 if the files are created and 0 if that was not possible
int create_open_files(const char *aid, const char *uid, char *fdata);

/* verifica se excedeu o tempo e cria o ficheiro */

/// @brief Get the actual time and verify if the auction aid is expired,
/// if so create the END_(aid).txt file.
/// The time written in the end file is the time when the auction expired.
/// @param aid Auction ID
/// @param timeactive Time in seconds the auction should be active
/// @param starttime Time when the auction started
/// @return 1 if the file is created and 0 if that was not possible
int create_end_if_expired(const char *aid, int timeactive, long starttime);

/// @brief Get the actual time and create a END_(aid).txt file.
/// The time written in the end file is the actual time.
/// @param aid Aution ID
/// @param starttime Time when the auction started
/// @return 1 if the file is created and 0 if that was not possible
int create_end_close(const char *aid, long starttime);

/// @brief Get the actual time and create 2 files: one in ...(uid)/BIDDED directory and
/// other in ...(aid)/BIDS directory. Also write relevant data to this second file
/// @param uid User ID
/// @param aid Auction ID
/// @param value Bid value
/// @param starttime Time when the auctionn started
/// @return 1 if the files are created and 0 if that was not possible
int create_bid_files(const char *uid, const char *aid, const char *value, long starttime);

#endif // __FILE_CREATION_H__
