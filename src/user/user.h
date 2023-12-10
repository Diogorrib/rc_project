#ifndef __USER_H__
#define __USER_H__

#include "../common/constant.h"
#include "../common/utils.h"

/// @brief Verify the parameters from the start of User app, if present -p and -n
/// update IP and PORT to use, if not present default parameters are used
/// @param argc Number of command-line arguments
/// @param argv Array of command-line arguments
void filter_input(int argc, char **argv);

//////////////////////////////////////// HELPER FUNCTIONS TO CARRY OUT THE ////////////////////////////////////////////
//////////////////////////////////////// OPERATIONS GIVEN BY THE COMMANDS /////////////////////////////////////////////

/// @brief Checks if there is an uid and a password in the user aplication
/// @param command Command to kown what instruction we are checking
/// @return 1 if there is no uid or password, 0 ohterwise
int no_uid_pass(char *command);

/// @brief Save the login data (uid and password) and set logged_in to 1
/// @param aux_uid User ID
/// @param aux_pass User password
void user_login(const char *aux_uid, const char *aux_pass);

//////////////////////////////////////// COMMUNICATION WITH THE SERVER ////////////////////////////////////////////////

/// @brief Send buffer to AS and receive msg_received from AS using UDP
/// @param buffer Data buffer to be sent
/// @param size Size of the data buffer
/// @param msg_received Buffer to store the received message
/// @return -1 the connection to AS went wrong, 0 otherwise
int udp(char *buffer, size_t size, char *msg_received);

/// @brief Send buffer to AS and receive msg_received from 
/// AS using TCP. This function can also send or receive files if fname != NULL
/// @param msg_sent Data buffer to be sent
/// @param fname File name (can be NULL)
/// @param size Size of the data buffer
/// @param msg_received Buffer to store the received message
/// @return -1 the connection to AS went wrong, 0 otherwise
int tcp(char *msg_sent, char *fname, ssize_t size, char *msg_received);

//////////////////////////////////// THE FOLLOWING FUNCTIONS ARE USED TO CARRY OUT THE ////////////////////////////////
/////////////////////////// OPERATIONS GIVEN BY THE COMMANDS REFERRED TO IN THE PROJECT STATEMENT /////////////////////

/// @brief Deals with the user login situation
void login();

/// @brief Deals with the user logout situation
void logout();

/// @brief Deals with the user unregister situation
void unregister();

/// @brief Deals with openning an auction 
void open_auction();

/// @brief Deals with closing an auction
void close_auction();

/// @brief Deals with listing all the user auctions and theirs state
/// @param first_word  First word of the command
void myauctions(char *first_word);

/// @brief Deals with listing all the user bids and their state
/// @param first_word  First word of the command
void mybids(char *first_word);

/// @brief Deals with listing all the existing auctions and their state
/// @param first_word  First word of the command
void list(char *first_word);

/// @brief Deals with showing the details of a specific asset
/// @param first_word  First word of the command
void show_asset(char *first_word);

/// @brief Deals with placing a bid on an auction
/// @param first_word  First word of the command
void bid(char *first_word);

/// @brief Deals with showing details of all the auctions
/// @param first_word  First word of the command
void show_record(char *first_word);

#endif // __USER_H__
