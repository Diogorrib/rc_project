#include "utils.h"

////////////////////////////////////////// FILES AND DIRECTORIES //////////////////////////////////////////////////////

int get_file_size(const char *fname, long *fsize) {
    struct stat file_stat;
    int res_stat;

    /* get file status using stat function */
    res_stat = stat(fname, &file_stat);

    /* check for errors or if the file size is 0 */
    if (res_stat == -1 || file_stat.st_size == 0) {
        printf("Error opening file\n");
        return -1; // Return -1 to indicate failure
    }

    /* store the file size in the provided pointer */
    *fsize = file_stat.st_size;

    return 0;
}

int verify_directory(const char *dirname) {
    struct stat info;

    /* get directory status using stat function */
    if (stat(dirname, &info) != 0)
        return 0; // if stat fails, assume it's not a directory

    /* Check if it's a directory */
    if (S_ISDIR(info.st_mode))
        return 1; // return 1 to indicate it's a directory

    return 0;
}

int verify_file(const char *filename) {
    struct stat info;

    /* get file status using stat function */
    if (stat(filename, &info) == 0)
        return 1;   // return 1 to indicate it's a directory
    return 0;
}

int create_file(const char *filename, const char *dirname, const char *fdata) {
    FILE *fp;
    if(!verify_directory(dirname))      // verify if directory exists
        if (mkdir(dirname, 0700) == -1) // create directory
            return -1;
    fp = fopen(filename, "w"); // open file for writing
    if(fp == NULL){
        printf("Can't open file %s\n", filename);
        return -1;
    }
    if (fdata != NULL)              // verify if theres is data to write
        fprintf(fp, "%s", fdata);   // write data to file
    fclose(fp);                     
    return 0;
}

///////////////////////////////////////////// COMMUNICATION ///////////////////////////////////////////////////////////

int set_send_timeout(int fd, int timeout) {
    struct timeval send_timeout;

    /* set timeout values in seconds and microseconds*/
    send_timeout.tv_sec = timeout;
    send_timeout.tv_usec = 0;

    /* set the socket option for send timeout */
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout)) < 0)
        return -1;
    return 0;
}

int set_recv_timeout(int fd, int timeout) {
    struct timeval recv_timeout;

    /* set timeout values in seconds and microseconds*/
    recv_timeout.tv_sec = timeout;
    recv_timeout.tv_usec = 0;

    /* set the socket option for receive timeout */
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0)
        return -1;
    return 0;
}


int send_file(int fd, char *fname, long fsize) {
    char buffer[BUFSIZ];
    ssize_t nleft,nwritten;
    size_t bytesRead;
    char *ptr;
    struct sigaction act;

    /* ignore SIGPIPE to handle broken pipe errors gracefully */
    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;
    if (sigaction(SIGPIPE,&act,NULL) == -1) {   // error
        printf("ERR: send_file: sigaction\n");
        return -1;
    }

    /* open file for reading */
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("ERR: send_file: Error opening file %s for reading\n", fname);
        return -1;
    }

    /* send file data */
    while (fsize > 0) {
        bytesRead = fread(buffer, 1, BUFSIZ, file);
        if (bytesRead <= 0) {
            printf("ERR: send_file: did not send all file\n");
            fclose(file); return -1;
        }
        nleft=(ssize_t)bytesRead; ptr=buffer;
        while (nleft>0) {
            if (set_send_timeout(fd, SERVER_TCP_TIMEOUT) == -1) {
                printf("ERR: send_file: did not send all file\n");
                fclose(file); return -1;
            }
            nwritten=write(fd,ptr,(size_t)nleft);
            if(nwritten <= 0) {    // closed by peer or timeout event
                printf("ERR: send_file: did not send all file\n");
                fclose(file); return -1;
            }
            nleft-=nwritten; ptr+=nwritten;
        }
        fsize-=(long)bytesRead;
    }

    fclose(file);
    return 0;
}

int receive_file(int fd, char *fname, long fsize, int timeout) {
    char buffer[BUFSIZ];
    ssize_t bytes_received, nleft, to_read;

    /* open file for writing */
    FILE *file = fopen(fname, "w");
    if (file == NULL) {
        printf("ERR: opening file %s for writing\n", fname);
        return -1;
    }

    nleft=fsize; to_read=BUFSIZ;
    /* receive message from AS */
    while (nleft > 0) {     // try to read all the file (fsize)
        if (BUFSIZ > nleft) // only read the fsize in this loop
            to_read = nleft;
        if (set_recv_timeout(fd, timeout) == -1) {
            printf("ERR: receive_file: write timeout\n");
            return -1;
        }
        bytes_received=read(fd,buffer,(size_t)to_read);
        nleft-=bytes_received;
        if(bytes_received == -1){   //  error
            printf("ERR: receive_file: read\n");
            fclose(file); unlink(fname); return -1;
        } else if(bytes_received == 0) break;            // closed by peer
        fwrite(buffer, 1, (size_t)bytes_received, file); // write in the file what is in the buffer
    }
    if (nleft) {    // verify if the file was completely read if not delete the file
        printf("ERR: not all data in the file %s was received\n", fname);
        fclose(file); unlink(fname);
        return -1;
    }
    if (set_recv_timeout(fd, timeout) == -1) {
        printf("ERR: receive_file: write timeout\n");
        return -1;
    }
    bytes_received=read(fd,buffer,1); // read the last \n
    if(bytes_received == -1 || buffer[0] != '\n') {   // error or not received the last \n
        printf("ERR: receive_file: read\n");
        fclose(file); unlink(fname);
        return -1;
    }
    fclose(file);
    return 0;
}

void vmode_request(const char *msg, const char *host, const char *service, int mode) {
    if(mode) {  // verify it the mode is enable
        printf("Request: %s", msg);
        printf(" | sent by IP: %s PORT: %s\n",host,service);
    }
}

void vmode_response(const char *msg, const char *host, const char *service, int mode) {
    if(mode) {  // verify it the mode is enable
        printf("Response: %s", msg);
        printf(" | sent to IP: %s PORT: %s\n",host,service);
    }
}

void vmode_more_info(const char *msg, int mode) {
    /* verify it the mode is enable */
    if(mode) printf("%s", msg);
}

///////////////////////////////////////////// STRING VERIFICATION ////////////////////////////////////////////////////

int is_numeric(const char *buffer) {
    /* get the length of the input string */
    size_t len = strlen(buffer);

    /* iterate through each character in the string */
    for (int i = 0; i < len; i++) {
        /* verify if the character is not a digit */
        if (!isdigit(buffer[i]))
            return 0; 
    }
    return 1; 
}

int is_alphanumeric(const char *buffer) {
    /* get the length of the input string */
    size_t len = strlen(buffer);

    /* iterate through each character in the string */
    for (int i = 0; i < len; i++) {
        /* verify if the character is neither a digit nor an alphabet character */
        if (!isdigit(buffer[i]) && !isalpha(buffer[i]))
            return 0;
    }
    return 1;
}

int is_alphanumeric_extra(const char *buffer) {
    /* get the length of the input string */
    size_t len = strlen(buffer);

    /* iterate through each character in the string */
    for (int i = 0; i < len; i++) {
        /* verify if the character is neither a digit, nor an alphabet character,
        nor one of the allowed extra characters */
        if (!isdigit(buffer[i]) && !isalpha(buffer[i]) &&
            buffer[i] != '-' && buffer[i] != '_' && buffer[i] != '.')
            return 0;
    }
    return 1;
}

int isDateTime(const char *buffer) {
    int year, month, day, hour, minute, second;
    /* try to parse the input string as a date and time */
    if (sscanf(buffer, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second) == 6) {
        /* verify if the parsed values represent a valid date and time */
        if (year >= 1900 && month >= 1 && month <= 12 && day >= 1 && day <= 31)
            return 1;
    }
    return 0;
}
