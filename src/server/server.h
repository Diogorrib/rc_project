#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/// @brief  Verify the parameters from the start of AS app, if present -p and -v
/// update PORT to use and turn on the verbose mode, if not present default
/// PORT is used and verbose mode remains off
/// @param argc
/// @param argv
void filter_input(int argc, char **argv);

///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////

void login(char *buffer, char *msg, char *host, char *service);
void logout(char *buffer, char *msg, char *host, char *service);
void unregister(char *buffer, char *msg, char *host, char *service);
void myauctions(char *buffer, char *msg, char *host, char *service);
void mybids(char *buffer, char *msg, char *host, char *service);
void show_record(char *buffer, char *msg, char *host, char *service);

/// @brief Verify the possible actions for udp communication
/// @param buffer 
/// @param msg 
/// @param addr 
/// @param addrlen 
void parse_udp_buffer(char *buffer, char *msg, struct sockaddr_in addr, socklen_t addrlen);

/// @brief Wait for udp requests and then process them
void udp();

///////////////////////////////////////////////// TCP //////////////////////////////////////////////////////////////////

/// @brief Read a sequence of characters until find a space or newline or exceded the limit (to_read)
/// @param fd 
/// @param buffer 
/// @param to_read 
/// @return -1 in case of error or if the string is empty or if the limit was exceded
int read_from_tcp(int fd, char *buffer, int to_read);

/// @brief Write the buffer to the tcp socket
/// @param fd 
/// @param buffer 
/// @return -1 in case of error
int write_to_tcp(int fd, char *buffer);

int open_auction(int fd, char *buffer, char *host, char *service);
int close_auction(int fd, char *buffer, char *host, char *service);
int show_asset(int fd, char *buffer, char *host, char *service);
int bid(int fd, char *buffer, char *host, char *service);

/// @brief Verify the possible actions for tcp communication
/// @param buffer 
/// @param msg 
/// @param addr 
/// @param addrlen 
void parse_tcp_buffer(int fd, char *buffer, struct sockaddr_in addr, socklen_t addrlen);

/// @brief Wait for tcp requests and then process them
void tcp();

#endif // __SERVER_H__
