#include "../common/constant.h"
#include "../common/utils.h"
#include "parse_user.h"

int confirm_login_input(char *buffer, char *uid, char *pass) {
    size_t cmd_size = strlen("login ");

    /* verify if the string has the correct size */
    if (strlen(buffer) != cmd_size+UID+PASSWORD+2) {
        printf("incorrect login attempt\n");
        return -1;
    }
    /* verify if spaces are placed correctly */
    if(buffer[cmd_size-1] != ' ' || buffer[cmd_size+UID] != ' ') {
        printf("incorrect login attempt\n");
        return -1;
    }
    memset(uid, '\0', UID+1); // initialize the uid with \0 in every index
    memset(pass, '\0', PASSWORD+1); // initialize the pass with \0 in every index
    sscanf(buffer+cmd_size, "%6s", uid);
    sscanf(buffer+cmd_size+UID+1, "%8s", pass);

    /* verify if the uid and pass have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        printf("incorrect login attempt\n");
        return -1;
    }
    /* verify if the uid is only digits and the pass is only letters and digits */
    if (!is_numeric(uid) || !is_alphanumeric(pass)) {
        printf("incorrect login attempt\n");
        return -1;
    }
    return 0;
}

int confirm_only_cmd_input(char *buffer, const char *cmd) {
    size_t cmd_size = strlen(cmd);

    /* verify if the string has the correct size and a '\n' at the end */
    if (strlen(buffer) != cmd_size+1 || buffer[cmd_size] != '\n') {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }
    return 0;
}

char *confirm_open_input(char *buffer, char *name, int *start_value, int *timeactive) {
    char *fname;
    size_t cmd_size = strlen("open ");
    size_t fname_size = strlen(buffer)-cmd_size+1; //worst case all input except cmd is the same string
    size_t offset;
    char aux[11];
    

    fname = (char *)malloc(fname_size);
    memset(name, '\0', NAME+1); // initialize the name with \0 in every index
    memset(fname, '\0', fname_size); // initialize the fname with \0 in every index

    sscanf(buffer+cmd_size, "%10s", name);
    if(strlen(name) > NAME || !is_alphanumeric(name) ||
        buffer[strlen(name)+cmd_size] != ' ' || buffer[strlen(name)+cmd_size+1] == ' ') {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    offset = cmd_size+strlen(name)+1;
    sscanf(buffer+offset, "%s", fname);
    fname_size = strlen(fname);
    if(buffer[offset+fname_size] != ' ' || buffer[offset+fname_size+1] == ' ' || buffer[offset+fname_size+1] == '-') {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    offset += fname_size+1;
    if (sscanf(buffer+offset, "%d", start_value) != 1) {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    sprintf(aux, "%d", *start_value);
    if(buffer[offset+strlen(aux)] != ' ' || buffer[offset+strlen(aux)+1] == ' ' || buffer[offset+strlen(aux)+1] == '-') {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    offset += strlen(aux)+1;
    if (sscanf(buffer+offset, "%d", timeactive) != 1) {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    sprintf(aux, "%d", *timeactive);
    if(buffer[offset+strlen(aux)] != '\n' || buffer[offset+strlen(aux)+1] != '\0') {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }
    if (*start_value > 999999 || *timeactive > 99999) {
        printf("incorrect open attempt\n");
        free(fname);
        return NULL;
    }

    return fname;
}