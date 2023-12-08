#ifndef __PARSE_USER_H__
#define __PARSE_USER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// GENERAL FUNCTIONS ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief Verify if the input is only a command 
/// plus the aid and new line character,
/// if input is not correct return is -1
/// @param buffer Buffer that stores the message receive in the command line
/// @param cmd Buffer thats receives the especific command that it is being tested
/// @param aid Buffer thats going to receive the aid of the auction
int confirm_aid_input(char *buffer, char *cmd, char *aid);

/// @brief Verify if the input is 
/// only a command and new line character,
/// if input is not correct return is -1
/// @param buffer Buffer that stores the message receive in the command line
/// @param cmd Buffer thats receives the especific command that it is being tested
int confirm_only_cmd_input(char *buffer, const char *cmd);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// FUNCTIONS FOR A SPECIFIC COMMAND /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// @brief If login input is not correct return is -1
/// @param buffer Buffer that stores the message receive in the command line
/// @param uid Buffer thats going to receive the uid of the user
/// @param pass Buffer thats going to receive the password of the user
int confirm_login_input(char *buffer, char *uid, char *pass);

/// @brief If open input is not correct return is -1
/// @param buffer Buffer that stores the message receive in the command line
/// @param name Buffer thats going to receive the description name of the auction
/// @param fname Buffer thats going to receive the filename of the auction
/// @param start_value Buffer thats going to receive the start value of the auction
/// @param timeactive Buffer thats going to receive the time that the auction is going to be active
int confirm_open_input(char *buffer, char *name, char *fname, int *start_value, int *timeactive);

/// @brief Verify if command plus aid plus 
/// the bid_value and new line character
/// if input is not correct return is -1
/// @param buffer Buffer that stores the message receive in the command line
/// @param cmd Buffer thats receives the especific command that it is being tested
/// @param aid Buffer thats going to receive the aid of the auction
/// @param bid_value Buffer thats going to receive the bid value of the auction
int confirm_bid_input(char *buffer, char *cmd, char *aid, char *bid_value);

#endif // __PARSE_USER_H__
