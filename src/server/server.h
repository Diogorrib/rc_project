#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/// @brief  Verify the parameters from the start of AS app, if present -p and -v
/// update PORT to use and turn on the verbose mode, if not present default
/// PORT is used and verbose mode remains off
/// @param argc Number of command-line arguments
/// @param argv Array of command-line arguments
void filter_input(int argc, char **argv);

///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////

/// @brief Deals with the server login situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void login(char *buffer, char *msg, char *host, char *service);

/// @brief Deals with the server logout situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void logout(char *buffer, char *msg, char *host, char *service);

/// @brief Deals with the server unregister situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void unregister(char *buffer, char *msg, char *host, char *service);

/// @brief Deals with the server listing of user auctions situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void myauctions(char *buffer, char *msg, char *host, char *service);

/// @brief Deals with the server listing of user bids situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void mybids(char *buffer, char *msg, char *host, char *service);

/// @brief Deals with the server listing of an auction situation
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void list(char *msg, char *host, char *service);

/// @brief Deals with the server showing all the auction information situation
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
void show_record(char *buffer, char *msg, char *host, char *service);

/// @brief Verify the possible actions for udp communication
/// @param buffer Buffer that stores the message received from the user
/// @param msg Buffer that is going to receive the message to send to the user
/// @param addr Address structure containing information about the sender
/// @param addrlen Size of the address structure 
void parse_udp_buffer(char *buffer, char *msg, struct sockaddr_in addr, socklen_t addrlen);

/// @brief Wait for udp requests and then process them
void udp();

///////////////////////////////////////////////// TCP //////////////////////////////////////////////////////////////////

/// @brief Read a sequence of characters until find a space or newline or exceded the limit (to_read)
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param to_read Size of the string to read including the space or newline
/// @return -1 in case of error or if the string is empty or if the limit was exceded
int read_from_tcp(int fd, char *buffer, int to_read);

/// @brief Write the buffer to the tcp socket
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message to send to the user
/// @return -1 in case of error
int write_to_tcp(int fd, char *buffer);

/// @brief Deals with openning an auction 
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
/// @return -1 in case of error, 0 if the instruction can't be executed or 1 if the instruction can be executed
int open_auction(int fd, char *buffer, char *host, char *service);

/// @brief Deals with closing an auction 
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
/// @return -1 in case of error, 0 if the instruction can't be executed or 1 if the instruction can be executed
int close_auction(int fd, char *buffer, char *host, char *service);

/// @brief Deals with showing the details of a specific asset
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent
/// @return -1 in case of error, 0 if the instruction can't be executed or 1 if the instruction can be executed
int show_asset(int fd, char *buffer, char *host, char *service);

/// @brief Deals with placing a bid on an auction
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param host IP where the message is going to be sent
/// @param service PORT where the message is going to be sent 
/// @return -1 in case of error, 0 if the instruction can't be executed or 1 if the instruction can be executed
int bid(int fd, char *buffer, char *host, char *service);

/// @brief Verify the possible actions for tcp communication
/// @param fd File descriptor of the socket to communicate with the user using the TCP protocol
/// @param buffer Buffer that stores the message received from the user
/// @param addr Address structure containing information about the sender
/// @param addrlen Size of the address structure
void parse_tcp_buffer(int fd, char *buffer, struct sockaddr_in addr, socklen_t addrlen);

/// @brief Wait for tcp requests and then process them
void tcp();

#endif // __SERVER_H__
