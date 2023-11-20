#include "../common/constant.h"
#include "../common/utils.h"
#include "process_user.h"

void get_cmd_status(char *msg, char * cmd, char *status) {
    memcpy(cmd, msg, CMD_N_SPACE);
    cmd[CMD_N_SPACE] = '\0';
    memcpy(status, msg+CMD_N_SPACE, STATUS);
    status[STATUS] = '\0';
}

int process_login(char *msg, char *uid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];

    get_cmd_status(msg, command, status);

    if(strcmp(command, "RLI ") || msg[8] != '\0') {
        printf("%s", msg);
        return 0;
    }
    if(!strcmp(status, "OK\n") && msg[7] == '\0') {
        printf("successful login\n");
        return 1;
    }
    if(!strcmp(status, "NOK\n")) {
        printf("incorrect login attempt\n");
        return 0;
    }
    if(!strcmp(status, "REG\n")) {
        printf("new %s registered\n", uid);
        return 1;
    }
    printf("%s", msg);
    return 0;
}

int process_logout(char *msg, const char *uid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];

    get_cmd_status(msg, command, status);

    if(strcmp(command, "RLO ") || msg[8] != '\0') {
        printf("%s", msg);
        return 0;
    }
    if(!strcmp(status, "OK\n") && msg[7] == '\0') {
        printf("successful logout\n");
        return 1;
    }
    if(!strcmp(status, "NOK\n")) {
        printf("%s not logged in\n", uid);
        return 0;
    }
    if(!strcmp(status, "UNR\n")) {
        printf("unknown %s\n", uid);
        return 0;
    }
    printf("%s", msg);
    return 0;
}

int process_unregister(char *msg, const char *uid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];

    get_cmd_status(msg, command, status);

    if(strcmp(command, "RUR ") || msg[8] != '\0') {
        printf("%s", msg);
        return 0;
    }
    if(!strcmp(status, "OK\n") && msg[7] == '\0') {
        printf("successful unregister\n");
        return 1;
    }
    if(!strcmp(status, "NOK\n")) {
        printf("incorrect unregister attempt\n");
        return 0;
    }
    if(!strcmp(status, "UNR\n")) {
        printf("unknown %s\n", uid);
        return 0;
    }
    printf("%s", msg);
    return 0;
}

void append_auction(char *string, char *auction) {
    char aux[LST_PRINT+1]; // 3 digits + " not active\n" (worst case) + '\0'
    
    memcpy(aux, auction, AID+1); // AID + 1 space
    if (auction[AID+1] == '0') {
        memcpy(aux+AID+1, "not active\n", 11);
        aux[LST_PRINT] = '\0';
    }
    else if (auction[AID+1] == '1') {
        memcpy(aux+AID+1, "active\n", 7);
        aux[11] = '\0';
    }
    strcpy(string + strlen(string), aux);
}

int get_list(char *destination, char *msg) {
    char auction[7];
    auction[6] = '\0';

    for (int i = 0; i < MAX_AUCTION; i++) {
        /* get an auction (AID + 1 space + state + (1 space or \n)) from msg */
        memcpy(auction, msg + 7 + (i*6), 6);
        if (confirm_list(msg, auction, i)) {
            printf("%s", msg);
            return -1;
        }
        append_auction(destination, auction);
        if (auction[5] == '\n') break; // no more auctions
    }
    return 0;
}

int confirm_list(char *msg, char *auction, int index) {
    /* verify if spaces and \n are correctly placed */
    if (auction[3] != ' ' || (auction[5] != ' ' && auction[5] != '\n') || (auction[5] == '\n' && msg[13 + (index*6)] != '\0'))
        return 1;
    /* verify if the AID is numeric */
    for (int i = 0; i < 3; i++) {
        if (!isdigit(auction[i]))
            return 1;
    }
    /* verify if state is 0 or 1 and if AID is between 001 and 999 (is not 000) */
    return ((auction[4] != '0' && auction[4] != '1') || (auction[0] == '0' && auction[1] == '0' && auction[2] == '0'));
}

void process_list(char *msg) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char auctions[LST_PRINT*MAX_AUCTION+1];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3

    memset(auctions, '\0', LST_PRINT*MAX_AUCTION+1);
    if(strcmp(command, "RLS "))
        printf("%s", msg);

    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("no auction was yet started\n");

    else if(!strcmp(status, "OK ")) {
        if (get_list(auctions, msg) == -1)
            return;
        printf("%s", auctions);
    }
    else printf("%s", msg);
}

int confirm_open(char *msg) {
    int initial = CMD_N_SPACE+STATUS-1;
    /* verify if the last characters are placed correctly */
    if (msg[10] != '\n' || msg[11] != '\0')
        return 0;
    /* verify if the AID is numeric */
    for (int i = initial; i < initial+3; i++) {
        if (!isdigit(msg[i]))
            return 0;
    }
    /* verify if AID is between 001 and 999 (is not 000) */
    return !(msg[initial] == '0' && msg[initial+1] == '0' && msg[initial+2] == '0');
}

void process_open(char *msg) {
    char command[CMD_N_SPACE+1], status[STATUS+1], aid[AID+2];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3

    if(strcmp(command, "ROA "))
        printf("%s", msg); 

    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("auction could not be started\n");

    else if(!strcmp(status, "NLG") && msg[7] == '\n' && msg[8] == '\0')
        printf("login is needed to open an auction\n");

    else if(!strcmp(status, "OK ") && confirm_open(msg)) {
        memcpy(aid, msg+CMD_N_SPACE+STATUS-1, AID+1);  // AID including \n
        aid[AID+1] = '\0';
        printf("auction started successfully with the identifier %s", aid);
    }
    else printf("%s", msg);
}

void process_close(char *msg, char *aid, const char *uid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    
    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3

    if(strcmp(command, "RCL ") || msg[8] != '\0')
        printf("%s", msg);

    else if(!strcmp(status, "OK\n") && msg[7] == '\0')
        printf("auction was successfully closed\n");

    else if(!strcmp(status, "NLG") && msg[7] == '\n')
        printf("login is needed to close an auction\n");

    else if(!strcmp(status, "EAU") && msg[7] == '\n')
        printf("auction %s does not exist\n", aid);

    else if(!strcmp(status, "EOW") && msg[7] == '\n')
        printf("auction is not owned by user %s\n", uid);

    else if(!strcmp(status, "END") && msg[7] == '\n')
        printf("auction time had already ended\n");

    else printf("%s", msg);
}