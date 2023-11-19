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
#include <ctype.h>

char *get_file_info(const char *fname, long *fsize);

int is_numeric(char *buffer);
int is_alphanumeric(char *buffer);

#endif // __UTILS_H__
