#include "../common/constant.h"
#include "../common/utils.h"
#include "parse_server.h"

/////////////////////////////////////////// UDP ///////////////////////////////////////////////////////////////////////

int confirm_login(const char *buffer, char *uid, char *pass, char *msg){

    /* verify if the string has the correct size */
    if (strlen(buffer) != LOGIN_SND-1) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    /* verify if spaces and '\n' are placed correctly */
    if(buffer[CMD_N_SPACE+UID] != ' ' || buffer[LOGIN_SND-2] != '\n') {
        sprintf(msg, "ERR\n");
        return -1;
    }
    
    memset(uid, '\0', UID+1);                           // initialize the UID with \0 in every index
    memset(pass, '\0', PASSWORD+1);                     // initialize the password with \0 in every index
    memcpy(uid, buffer+CMD_N_SPACE, UID);               // stores the UID received from the user
    memcpy(pass, buffer+CMD_N_SPACE+UID+1, PASSWORD);   // stores the password received from the user

    /* verify if the UID and password have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    /* verify if the UID is only digits and the password is only letters and digits */
    if (!is_numeric(uid) || !is_alphanumeric(pass)) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    return 0;
}

int confirm_list_my(const char *buffer, char *uid, char *msg) {

    /* verify if the string has the correct size */
    if (strlen(buffer) != MY_SND-1) {
        sprintf(msg, "ERR\n");
        return -1;
    }

    /* verify if the '\n' is placed correctly */
    if(buffer[CMD_N_SPACE+UID] != '\n') {
        sprintf(msg, "ERR\n");
        return -1;
    }

    memset(uid, '\0', UID+1);               // initialize the UID with \0 in every index
    memcpy(uid, buffer+CMD_N_SPACE, UID);   // stores the UID received from the user

    /* verify if the UID have the correct size and is only digits */
    if(strlen(uid) != UID || !is_numeric(uid)) {
        sprintf(msg, "ERR\n");
        return -1;
    }

    return 0;
}

int confirm_sr(const char *buffer, char *aid, char *msg) {

    /* verify if the string has the correct size */
    if (strlen(buffer) != SHOW_SND-1) {
        sprintf(msg, "ERR\n");
        return -1;
    }

    /* verify if the '\n' is placed correctly */
    if(buffer[CMD_N_SPACE+AID] != '\n') {
        sprintf(msg, "ERR\n");
        return -1;
    }

    memset(aid, '\0', AID+1);               // initialize the aid with \0 in every index
    memcpy(aid, buffer+CMD_N_SPACE, AID);   // stores the aid received from the user

    /* verify if the aid have the correct size and is only digits */
    if(strlen(aid) != AID || !is_numeric(aid)) {
        sprintf(msg, "ERR\n");
        return -1;
    }

    return 0;
}

/////////////////////////////////////////// TCP ///////////////////////////////////////////////////////////////////////

int confirm_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *fsize, char *msg) {

    /* verify if the UID and password have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        sprintf(msg, "ERR\n");
        return -1;
    }

    /* verify all the fields */
    if (!is_numeric(uid) || !is_alphanumeric(pass) || !is_alphanumeric_extra(name) ||
        !is_numeric(start_value) || !is_numeric(timeactive) || !is_alphanumeric_extra(fname) ||
        !is_numeric(fsize)) {
        sprintf(msg, "ERR\n");
        return -1;
    }
    return 0;
}

int confirm_close(const char *uid, const char *pass, const char *aid, char *buffer) {

    /* verify if the UID, password and aid have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD || strlen(aid) != AID) {
        sprintf(buffer, "ERR\n");
        return -1;
    }

    /* verify all the fields */
    if (!is_numeric(uid) || !is_alphanumeric(pass) || !is_numeric(aid)) {
        sprintf(buffer, "ERR\n");
        return -1;
    }
    return 0;
}

int confirm_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer) {

    /* verify if the UID, password and aid have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD || strlen(aid) != AID) {
        sprintf(buffer, "ERR\n");
        return -1;
    }
    
    /* verify all the fields */
    if (!is_numeric(uid) || !is_alphanumeric(pass) || !is_numeric(aid) || !is_numeric(bid_value)) {
        sprintf(buffer, "ERR\n");
        return -1;
    }
    return 0;
}

int confirm_sa(const char *aid, char *buffer) {
    
    /* verify if the aid have the correct size and is only digits */
    if(strlen(aid) != AID || !is_numeric(aid)) {
        sprintf(buffer, "ERR\n");
        return -1;
    }

    return 0;
}
