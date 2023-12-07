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

void login(char *buffer, char *msg);
void logout(char *buffer, char *msg);
void unregister(char *buffer, char *msg);
void myauctions(char *buffer, char *msg);
void mybids(char *buffer, char *msg);
void show_record(char *buffer, char *msg);

void parse_udp_buffer(char *buffer, char *msg, struct sockaddr_in addr, socklen_t addrlen);

void udp();

///////////////////////////////////////////////// TCP //////////////////////////////////////////////////////////////////

int read_from_tcp(int fd, char *buffer, int to_read);
int write_to_tcp(int fd, char *buffer);

void open_auction(int fd, char *buffer);
void close_auction(int fd, char *buffer);
/* if return is 0 message is NOK else send the asset (inside ths function) */
int show_asset(int fd, char *buffer);
void bid(int fd, char *buffer);

void parse_tcp_buffer(int fd, char *buffer, struct sockaddr_in addr, socklen_t addrlen);

void tcp();

#endif // __SERVER_H__
