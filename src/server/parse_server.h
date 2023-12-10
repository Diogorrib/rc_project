#ifndef __PARSE_SERVER_H__
#define __PARSE_SERVER_H__

/////////////////////////////////////////// UDP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if the buffer has the correct size and has a uid and pass
/// @param buffer 
/// @param uid 
/// @param pass 
/// @param msg 
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_login(const char *buffer, char *uid, char *pass, char *msg);

/// @brief Verify if the buffer has the correct size and has a uid
/// @param buffer 
/// @param uid 
/// @param msg 
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_list_my(const char *buffer, char *uid, char *msg);

/// @brief Verify if the buffer has the correct size and has a aid
/// @param buffer 
/// @param aid 
/// @param msg 
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_sr(const char *buffer, char *aid, char *msg);

/////////////////////////////////////////// TCP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if all the fields of the message received are correct
/// @param uid 
/// @param pass 
/// @param name 
/// @param start_value 
/// @param timeactive 
/// @param fname 
/// @param fsize 
/// @param msg 
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *fsize, char *msg);

/// @brief Verify if all the fields of the message received are correct
/// @param uid 
/// @param pass 
/// @param aid 
/// @param buffer 
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_close(const char *uid, const char *pass, const char *aid, char *buffer);

/// @brief Verify if all the fields of the message received are correct
/// @param uid 
/// @param pass 
/// @param aid 
/// @param bid_value 
/// @param buffer 
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer);

/// @brief Verify if the aid correct
/// @param aid 
/// @param buffer 
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_sa(const char *aid, char *buffer);


#endif // __PARSE_SERVER_H__