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
#include <sys/stat.h>

/** Get the size of the file, if return is -1 it is possible that fname is not
 * the name of a file or this file does not exist
 */
int get_file_size(const char *fname, long *fsize);

/** Delete a file, return -1 if an error occurs
 */
int delete_file(char *fname);

/** Open and read a file and send it using tcp,
 * return is -1 in case of error
 */
int send_file(int fd, char *fname);

/** Open a file and while the file is not completely written,
 * writes to the file what is received using tcp,
 * return is -1 in case of error and file is also deleted
 */
int receive_file(int fd, char *fname, long fsize);

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

int verify_directory(const char *dirname);

int verify_file(const char *filename);

int create_file(const char *filename, const char *dirname, const char *fdata);

void request_received(const char *msg, char *host, char *service, int mode);

#endif // __UTILS_H__
