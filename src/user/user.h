#ifndef __USER_H__
#define __USER_H__

#include "../common/utils.h"
#include "../common/constant.h"

/* struct Login {
    char uid[7];
    int logged_in = 0;
}; */

void filter_input(int argc, char **argv);
void udp(char *buffer, size_t size, char *msg_received);
void login();

#endif // __USER_H__
