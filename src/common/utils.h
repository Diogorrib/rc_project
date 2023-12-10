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
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <dirent.h>

#define SERVER_UDP_TIMEOUT 3
#define SERVER_TCP_TIMEOUT 2

////////////////////////////////////////// FILES AND DIRECTORIES //////////////////////////////////////////////////////

/// @brief Get the size of the file in bytes
/// @param fname 
/// @param fsize 
/// @return -1 indicates that it is possible that fname is not the name of a file or this file does not exist
int get_file_size(const char *fname, long *fsize);

/// @brief Verify if the directory exists
/// @param dirname 
/// @return 1 if true 0 if false
int verify_directory(const char *dirname);

/// @brief Verify if the file exists
/// @param filename 
/// @return 1 if true 0 if false
int verify_file(const char *filename);

/// @brief Create a file and its directory (only 1 directory)
/// @param filename 
/// @param dirname 
/// @param fdata 
/// @return -1 in case of error
int create_file(const char *filename, const char *dirname, const char *fdata);

///////////////////////////////////////////// COMMUNICATION ///////////////////////////////////////////////////////////

/// @brief Set send timeout
/// @param fd 
/// @param timeout 
/// @return -1 in case of error
int set_send_timeout(int fd, int timeout);

/// @brief Set receive timeout
/// @param fd 
/// @param timeout 
/// @return -1 in case of error
int set_recv_timeout(int fd, int timeout);

/// @brief Open and read a file and send it using tcp
/// @param fd 
/// @param fname 
/// @param fsize 
/// @return -1 in case of error
int send_file(int fd, char *fname, long fsize);

/// @brief Open a file and while the file is not completely written,
/// writes to the file what is received using tcp
/// @param fd 
/// @param fname 
/// @param fsize 
/// @param timeout 
/// @return -1 in case of error and file is also deleted
int receive_file(int fd, char *fname, long fsize, int timeout);

/// @brief Print the verbose mode message if it is active
/// @param msg 
/// @param host 
/// @param service 
/// @param mode 
void vmode_request(const char *msg, const char *host, const char *service, int mode);

/// @brief Print the verbose mode message if it is active
/// @param msg 
/// @param host 
/// @param service 
/// @param mode 
void vmode_response(const char *msg, const char *host, const char *service, int mode);

/// @brief Print the verbose mode message if it is active
/// @param msg 
/// @param mode 
void vmode_more_info(const char *msg, int mode);

///////////////////////////////////////////// STRING VERIFICATION ////////////////////////////////////////////////////

/// @brief Verify if the buffer is only digits
/// @param buffer 
/// @return 1 if true 0 if false
int is_numeric(const char *buffer);

/// @brief Verify if the buffer is only digits and letters
/// @param buffer 
/// @return 1 if true 0 if false
int is_alphanumeric(const char *buffer);

/// @brief Verify if the buffer is only digits and letters plus '-', '_' and '.'
/// @param buffer 
/// @return 1 if true 0 if false
int is_alphanumeric_extra(const char *buffer);

/// @brief Verify if the buffer is a date time string
/// @param buffer 
/// @return 1 if true 0 if false
int isDateTime(const char *buffer);

#endif // __UTILS_H__
