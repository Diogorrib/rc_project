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
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

/** Get the size of the file, if return is -1 it is possible that fname is not
 * the name of a file or this file does not exist
 */
int get_file_size(const char *fname, long *fsize);

/** Open and read a file and send it using tcp,
 * return is -1 in case of error
 */
int send_file(int fd, char *fname);

/** Verify if the buffer is only digits
 */
int is_numeric(char *buffer);

/** Verify if the buffer is only digits and letters
 */
int is_alphanumeric(char *buffer);

/** Verify if the buffer is only digits and letters plus '-', '_' and '.'
 */
int is_alphanumeric_extra(char *buffer);

/** Verify if the buffer is a date time string
 */
int isDateTime(const char *buffer);

#endif // __UTILS_H__
