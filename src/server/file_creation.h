#ifndef __FILE_CREATION_H__
#define __FILE_CREATION_H__

////////////////////////////////////// USERS DIRECTORY FILES //////////////////////////////////////////////////////////
/* The following functions return 1 if the file is created and 0 if that was not possible,
  functions with fdata also write that to the file */

int create_login(const char *uid);
int create_password(const char *uid, const char *pass);
int create_hosted(const char *uid, const char *aid);
int create_bidded(const char *uid, const char *aid);

////////////////////////////////////// AUCTIONS DIRECTORY FILES ///////////////////////////////////////////////////////
/* The following functions return 1 if the file is created and 0 if that was not possible,
  functions with fdata also write that to the file */

int create_start(const char *aid, const char *fdata);
int create_asset(int fd, const char *aid, const char *fname, const char *fsize);
int create_end(const char *aid, char *fdata);
int create_bid_value(const char *aid, int bid_value, char *fdata);

//////////////////////////////////// MULTIPLE FILE CREATION OR CONDITIONAL ////////////////////////////////////////////

/// @brief Get the actual time and create 2 files: one in ...(uid)/HOSTED directory and
/// other in AUCTIONS/(aid) directory. Also write fdata + the actual time to this second file
/// @param aid 
/// @param uid 
/// @param fdata 
/// @return 1 if the files are created and 0 if that was not possible
int create_open_files(const char *aid, const char *uid, char *fdata);

/* verifica se excedeu o tempo e cria o ficheiro */

/// @brief Get the actual time and verify if the auction aid is expired,
/// if so create the END_(aid).txt file.
/// The time written in the end file is the time when the auction expired.
/// @param aid 
/// @param timeactive 
/// @param starttime 
/// @return 1 if the file is created and 0 if that was not possible
int create_end_if_expired(const char *aid, int timeactive, long starttime);

/// @brief Get the actual time and create a END_(aid).txt file.
/// The time written in the end file is the actual time.
/// @param aid 
/// @param starttime 
/// @return 1 if the file is created and 0 if that was not possible
int create_end_close(const char *aid, long starttime);

/// @brief Get the actual time and create 2 files: one in ...(uid)/BIDDED directory and
/// other in ...(aid)/BIDS directory. Also write relevant data to this second file
/// @param uid 
/// @param aid 
/// @param value 
/// @param starttime 
/// @return 1 if the files are created and 0 if that was not possible
int create_bid_files(const char *uid, const char *aid, const char *value, long starttime);

#endif // __FILE_CREATION_H__
