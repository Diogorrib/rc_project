#ifndef __SERVER_H__
#define __SERVER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/** Verify the parameters from the start of AS app, if present -p and -v
 * update PORT to use and turn on the verbose mode, if not present default
 * PORT is used and verbose mode remains off
 */
void filter_input(int argc, char **argv);









void login(char *buffer, char *msg);
void logout(char *buffer, char *msg);
void unregister(char *buffer, char *msg);
void myauctions(char *buffer, char *msg);
void mybids(char *buffer, char *msg);
void list(char *buffer, char *msg);
void show_record(char *buffer, char *msg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void parse_udp_buffer(char *buffer, char *msg);

void udp();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// TCP /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int read_from_tcp(int fd, char *buffer, long to_read);

void open_auction(int fd);
void close_auction(int fd);
void show_asset(int fd);
void bid(int fd);

void parse_tcp_buffer(int fd);

void tcp();

#endif // __SERVER_H__
