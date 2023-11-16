#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/** Return the concatenation of a first string (command) with the n_strings present in strings array
 * separated by space and with '\n' at the end
*/
char *build_string(const char *command, const char **strings, int n_strings);

#endif // __UTILS_H__
