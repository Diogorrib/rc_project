#include "utils.h"

int get_file_size(const char *fname, long *fsize) {
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    *fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fclose(file);
    return 0;
}

int send_file(int fd, char *fname) {
    char buffer[512];

    /* Open file for reading */
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("Error opening file %s for reading\n", fname);
        close(fd);
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
    struct tm tm;
    if (strptime(buffer, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
        int year = tm.tm_year + 1900;
        int month = tm.tm_mon + 1;
        int day = tm.tm_mday;

        if (year >= 1900 && month >= 1 && month <= 12 && day >= 1 && day <= 31)
            return 1;
    }
    return 0;
}
