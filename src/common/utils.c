#include "utils.h"

int get_file_size (const char *fname, long *fsize) {
    struct stat filestat;
    int res_stat;
    res_stat=stat(fname , &filestat) ;
    if (res_stat == -1 || filestat.st_size == 0) {
        printf("Error opening file\n");
        return -1;
    }
    *fsize = filestat.st_size;
    return 0;
}

int delete_file(char *fname) {
    if (strlen(fname) <= 0) return -1;
    unlink(fname);
    return 0;
}

int send_file(int fd, char *fname) {
    char buffer[512];

    /* Open file for reading */
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("Error opening file %s for reading\n", fname);
        return -1;
    }

    /* Send file data */
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(fd, buffer, bytesRead, 0);
    }

    fclose(file);
    return 0;
}

int receive_file(int fd, char *fname, long fsize) {
    char buffer[512];
    ssize_t bytes_received, nleft, to_read;

    /* Open file for writing */
    FILE *file = fopen(fname, "w");
    if (file == NULL) {
        printf("Error opening file %s for writing\n", fname);
        return -1;
    }

    nleft=fsize; to_read=sizeof(buffer);
    /* receive message from AS */
    while (nleft > 0) { // try to read all the file (fsize)
        if (sizeof(buffer) > nleft) // only read the fsize in this loop
            to_read = nleft;
        bytes_received=read(fd,buffer,(size_t)to_read);
        nleft-=bytes_received;
        if(bytes_received == -1){//error
            printf("Can't receive from server AS. Try again\n");
            fclose(file); delete_file(fname); return -1;
        } else if(bytes_received == 0) break; //closed by peer
        fwrite(buffer, 1, (size_t)bytes_received, file); // write in the file what is in the buffer
    }
    if (nleft) {    // verify if the file was completely read if not delete the file
        printf("not all data in the file %s was received\n", fname);
        fclose(file); delete_file(fname);
        return -1;
    }
    bytes_received=read(fd,buffer,1); // read the last \n
    if(bytes_received == -1 || buffer[0] != '\n') {   //error or not received the last \n
        printf("Can't receive from server AS. Try again\n");
        fclose(file); delete_file(fname);
        return -1;
    }
    fclose(file);
    return 0;
}

int is_numeric(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i])) return 0;
    }
    return 1;    
}

int is_alphanumeric(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i]) && !isalpha(buffer[i])) return 0;
    }
    return 1;
}

int is_alphanumeric_extra(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isdigit(buffer[i]) && !isalpha(buffer[i]) &&
            buffer[i] != '-' && buffer[i] != '_' && buffer[i] != '.')
            return 0;
    }
    return 1;
}

int isDateTime(const char *buffer) {
    int year, month, day, hour, minute, second;
    if (sscanf(buffer, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second) == 6) {
        if (year >= 1900 && month >= 1 && month <= 12 && day >= 1 && day <= 31)
            return 1;
    }
    return 0;
}

int verify_directory(const char *dirname) {
    struct stat info;
    
    if (stat(dirname, &info) != 0)
        return 0;
    if (S_ISDIR(info.st_mode))
        return 1;
    return 0;
}

int verify_file(const char *filename) {
    struct stat info;
    if (stat(filename, &info) == 0)
        return 1;
    return 0;
}


int create_file(const char *filename, const char *dirname, const char *fdata) {
    FILE *fp;
    if(!verify_directory(dirname))
        if (mkdir(dirname, 0700) == -1)
            return -1;
    fp=fopen (filename, "w");
    if(fp==NULL){
        printf("Can't open file %s\n", filename);
        return -1;
    }
    if (fdata != NULL)
        fprintf(fp, "%s", fdata);
    fclose(fp);
    return 0;
}

void request_received(const char *msg, char *host, char *service, int mode) {
    if(mode) {
        printf("Request: %s", msg);
        printf("       Sent by IP: %s\tPORT: %s]\n",host,service);
    }
}
