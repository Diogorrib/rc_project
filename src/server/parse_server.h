#ifndef __PARSE_SERVER_H__
#define __PARSE_SERVER_H__

/////////////////////////////////////////// UDP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if the buffer has the correct size and has a UID and pass
/// @param buffer Buffer that stores the message received from the user
/// @param uid Buffer that is going to receive the UID from the user
/// @param pass Buffer that is going to receive the password from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_login(const char *buffer, char *uid, char *pass, char *msg);

/// @brief Verify if the buffer has the correct size and has a UID
/// @param buffer Buffer that stores the message received from the user
/// @param uid Buffer that is going to receive the UID from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_list_my(const char *buffer, char *uid, char *msg);

/// @brief Verify if the buffer has the correct size and has a aid
/// @param buffer Buffer that stores the message received from the user
/// @param aid Buffer that is going to receive the aid from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @return -1 if input is not correct (write ERR to the message), 0 otherwise
int confirm_sr(const char *buffer, char *aid, char *msg);

/////////////////////////////////////////// TCP ///////////////////////////////////////////////////////////////////////

/// @brief Verify if all the fields of the message received are correct
/// @param uid Buffer that is going to receive the UID from the user
/// @param pass Buffer that is going to receive the password from the user
/// @param name Buffer that is going to receive the description name of the auction from the user
/// @param start_value Buffer that is going to receive the start value of the auction from the user
/// @param timeactive Buffer that is going to receive the time that the auction will be active from the user
/// @param fname Buffer that is going to receive the fname of the auction from the user
/// @param fsize Buffer that is going to receive the size of the asset to create from the user
/// @param msg Buffer that stores the message received from the user
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *fsize, char *msg);

/// @brief Verify if all the fields of the message received are correct
/// @param uid Buffer that is going to receive the UID from the user
/// @param pass Buffer that is going to receive the passsword from the user
/// @param aid Buffer that is going to receive the aid from the user
/// @param msg Buffer that stores the message received from the user
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_close(const char *uid, const char *pass, const char *aid, char *msg);

/// @brief Verify if all the fields of the message received are correct
/// @param uid Buffer that is going to receive the UID from the user
/// @param pass Buffer that is going to receive the password from the user
/// @param aid Buffer that is going to receive the aid from the user
/// @param bid_value Buffer that is going to receive the value of the bid from the user
/// @param msg Buffer that stores the message received from the user
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *msg);

/// @brief Verify if the aid correct
/// @param aid Buffer that is going to receive the aid from the user
/// @param msg Buffer that stores the message received from the user
/// @return -1 if input is not correct (write ERR to the buffer), 0 otherwise
int confirm_sa(const char *aid, char *msg);


#endif // __PARSE_SERVER_H__