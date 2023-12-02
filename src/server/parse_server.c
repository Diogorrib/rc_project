#include "../common/constant.h"
#include "../common/utils.h"
#include "parse_server.h"


int confirm_login(char *buffer, char *uid, char *pass, char *msg){

    /* verify if the string has the correct size */
    if (strlen(buffer) != LOGIN_SND-1) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    /* verify if spaces are placed correctly */
    if(buffer[CMD_N_SPACE+UID] != ' ' || buffer[LOGIN_SND-2] != '\n') {
        sprintf(msg, "ERR\n");
        return -1;
    }
    
    memset(uid, '\0', UID+1); // initialize the uid with \0 in every index
    memset(pass, '\0', PASSWORD+1); // initialize the pass with \0 in every index
    memcpy(uid, buffer+CMD_N_SPACE, UID);
    memcpy(pass, buffer+CMD_N_SPACE+UID+1, PASSWORD);

    /* verify if the uid and pass have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    /* verify if the uid is only digits and the pass is only letters and digits */
    if (!is_numeric(uid) || !is_alphanumeric(pass)) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    return 0;
}

int confirm_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *fsize, char *msg) {

    /* verify if the uid and pass have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    /* verify if the uid is only digits and the pass is only letters and digits */
    if (!is_numeric(uid) || !is_alphanumeric(pass) || !is_alphanumeric_extra(name) ||
        !is_numeric(start_value) || !is_numeric(timeactive) || !is_alphanumeric_extra(fname) ||
        !is_numeric(fsize)) {
        // TODO: verify *.xxx for filename
        sprintf(msg, "ERR\n");
        return -1;
    }
    return 0;
}
