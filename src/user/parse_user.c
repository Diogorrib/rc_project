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

int confirm_open_input(char *buffer, char *name, char *fname, int *start_value, int *timeactive) {
    char aux[11];
    size_t offset;
    size_t cmd_size = strlen("open ");
    
    memset(name, '\0', NAME+1); // initialize the name with \0 in every index
    memset(fname, '\0', FNAME+1); // initialize the fname with \0 in every index

    sscanf(buffer+cmd_size, "%10s", name);
    /* verify if the string has the correct size is only letters and numbers and if spaces are placed correctly */
    offset = cmd_size+strlen(name)+1;   // advance string
    if(strlen(name) > NAME || !is_alphanumeric(name) || buffer[offset-1] != ' ' || buffer[offset] == ' ') {
        printf("incorrect open attempt\n");
        return -1;
    }
    
    sscanf(buffer+offset, "%s", fname);
    offset += strlen(fname)+1; // advance string
    /* verify if the spaces are placed correctly */
    if(buffer[offset-1] != ' ' || buffer[offset] == ' ' || buffer[offset] == '-') {
        printf("incorrect open attempt\n");
        return -1;
    }
    
    /* verify if the string is only digits */
    if (sscanf(buffer+offset, "%d", start_value) != 1) {
        printf("incorrect open attempt\n");
        return -1;
    }
    sprintf(aux, "%d", *start_value);
    offset += strlen(aux)+1;    // advance string
    /* verify if the spaces are placed correctly and max of 6 digits */
    if(buffer[offset-1] != ' ' || buffer[offset] == ' ' || buffer[offset] == '-' || strlen(aux) > 6) {
        printf("incorrect open attempt\n");
        return -1;
    }
    
    /* verify if the string is only digits */
    if (sscanf(buffer+offset, "%d", timeactive) != 1) {
        printf("incorrect open attempt\n");
        return -1;
    }
    sprintf(aux, "%d", *timeactive);
    offset += strlen(aux)+1;    // advance string
    /* verify if the last characters are placed correctly and max of 5 digits */
    if(buffer[offset-1] != '\n' || buffer[offset] != '\0' || strlen(aux) > 5) {
        printf("incorrect open attempt\n");
        return -1;
    }
    return 0;
}

int confirm_aid_input(char *buffer, char *cmd, char *aid) {
    size_t cmd_size = strlen(cmd)+1; // cmd including the space

    /* verify if the string has the correct size */
    if (strlen(buffer) != cmd_size+AID+1) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }

    /* verify if spaces are placed correctly */
    if(buffer[cmd_size-1] != ' ' || buffer[cmd_size+AID] != '\n') {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }
    memset(aid, '\0', AID+1); // initialize the aid with \0 in every index
    sscanf(buffer+cmd_size, "%3s", aid);

    /* verify if the aid has the correct size */
    if(strlen(aid) != AID) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }
    /* verify if the aid is only digits */
    if (!is_numeric(aid)) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }
    if(aid[0] == '0' && aid[1] == '0' && aid[2] == '0') {
        printf("incorrect identifier\n");
        return -1;
    }
    return 0;
}

int confirm_bid_input(char *buffer, char *cmd, char *aid, char *bid_value) {
    size_t cmd_size = strlen(cmd)+1; // cmd including the space

    memset(aid, '\0', AID+1); // initialize the aid with \0 in every index
    memset(bid_value, '\0', MAX_4_SOME_INTS+1); // initialize the bid_value with \0 in every index

    sscanf(buffer+cmd_size, "%3s", aid);
    sscanf(buffer+cmd_size+AID+1, "%19s", bid_value);

    /* verify if the string has the correct size */
    if (strlen(buffer) != cmd_size+AID+strlen(bid_value)+2) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }

    /* verify if the string has only one space between the command and the aid and that the aid isn't negative */
    if(buffer[cmd_size-1] != ' ' || buffer[cmd_size] == '-' || buffer[cmd_size] == ' ') {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }

    /* verify if the aid has the correct size */
    if(strlen(aid) != AID) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }

    /* verify if the aid is only digits */
    if (!is_numeric(aid) || !is_numeric(bid_value)) {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }

    /* verify if the aid despite being a digit it's an invalid one */
    if(aid[0] == '0' && aid[1] == '0' && aid[2] == '0') {
        printf("incorrect identifier\n");
        return -1;
    }

    /* verify is there's only one space between aid and bid_value and that the bid_value isn't negative */
    if(buffer[cmd_size+AID] != ' ' || buffer[cmd_size+AID+1] == '-' || buffer[cmd_size+AID+1] == ' ') {
        printf("incorrect %s attempt\n", cmd);
        return -1;
    }    

    /* verify is there's only one space between aid and bid_value and that the bid_value isn't negative */
    if(buffer[cmd_size+AID+strlen(bid_value)+1] != '\n' || buffer[cmd_size+AID+strlen(bid_value)+2] != '\0') {
        printf("iAncorrect %s attempt\n", cmd);
        return -1;
    }    

    return 0;
}

