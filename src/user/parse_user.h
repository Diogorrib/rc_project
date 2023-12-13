#ifndef __PARSE_USER_H__
#define __PARSE_USER_H__

///////////////////////////////////////////// GENERAL FUNCTIONS ///////////////////////////////////////////////////////

/// @brief Verify if the input is only a command plus the aid and new line character
/// @param buffer Buffer that stores the message received in the command line
/// @param cmd Buffer that receives the especific command that it is being tested
/// @param aid Buffer that is going to receive the aid of the auction
/// @return -1 if input is not correct, 0 otherwise
int confirm_aid_input(char *buffer, char *cmd, char *aid);

/// @brief Verify if the input is only a command and new line character
/// @param buffer Buffer that stores the message received in the command line
/// @param cmd Buffer that receives the especific command that it is being tested
/// @return -1 if input is not correct, 0 otherwise
int confirm_only_cmd_input(char *buffer, const char *cmd);

//////////////////////////////////////// FUNCTIONS FOR A SPECIFIC COMMAND /////////////////////////////////////////////

/// @brief Verify command plus UID plus password and new line character
/// @param buffer Buffer that stores the message received in the command line
/// @param uid Buffer that is going to receive the UID of the user
/// @param pass Buffer that is going to receive the password of the user
/// @return -1 if input is not correct, 0 otherwise
int confirm_login_input(char *buffer, char *uid, char *pass);

/// @brief Verify command plus name plus fname plus starte value plus time active and new line character
/// @param buffer Buffer that stores the message received in the command line
/// @param name Buffer that is going to receive the description name of the auction
/// @param fname Buffer that is going to receive the filename of the auction
/// @param start_value Buffer that is going to receive the start value of the auction
/// @param timeactive Buffer that is going to receive the time that the auction is going to be active
/// @return -1 if input is not correct, 0 otherwise
int confirm_open_input(char *buffer, char *name, char *fname, int *start_value, int *timeactive);

/// @brief Verify command plus aid plus the bid_value and new line character
/// @param buffer Buffer that stores the message received in the command line
/// @param cmd Buffer that receives the especific command that it is being tested
/// @param aid Buffer that is going to receive the aid of the auction
/// @param bid_value Buffer that is going to receive the bid value of the auction
/// @return -1 if input is not correct, 0 otherwise
int confirm_bid_input(char *buffer, char *cmd, char *aid, char *bid_value);

#endif // __PARSE_USER_H__
