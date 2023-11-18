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
#include <signal.h>

/** Return the concatenation of a first string (command) with the n_strings present in strings array
 * separated by space and with newline character at the end
*/
void build_string(char *buffer, const char *command, const char **strings, int n_strings);

char *get_file_info(const char *fname);

#endif // __UTILS_H__
