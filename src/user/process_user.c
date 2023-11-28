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

void append_auction(char *string, char *auction) {
    char aux[LST_PRINT+1]; // worst case + '\0'
    
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

int get_auctions(char *destination, char *msg) {
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

    if(strcmp(command, "RLS "))
        printf("%s", msg);

    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("no auction was yet started\n");

    else if(!strcmp(status, "OK ")) {
        memset(auctions, '\0', LST_PRINT*MAX_AUCTION+1);
        if (get_auctions(auctions, msg) == -1)
            return;
        printf("Auctions List:\n%s", auctions);
    }
    else printf("%s", msg);
}

void process_ma(char *msg) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char auctions[LST_PRINT*MAX_AUCTION+1];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3

    if(strcmp(command, "RMA "))
        printf("%s", msg);
    
    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("you have no ongoing auctions\n");

    else if(!strcmp(status, "NLG") && msg[7] == '\n' && msg[8] == '\0')
        printf("please login first\n");

    else if(!strcmp(status, "OK ")) {
        memset(auctions, '\0', LST_PRINT*MAX_AUCTION+1);
        if (get_auctions(auctions, msg) == -1)
            return;
        printf("My Auctions:\n%s", auctions);
    }
    else printf("%s", msg);
}

void process_mb(char *msg) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char auctions[LST_PRINT*MAX_AUCTION+1];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3
    
    if(strcmp(command, "RMB "))
        printf("%s", msg);
    
    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("you have no ongoing bids\n");

    else if(!strcmp(status, "NLG") && msg[7] == '\n' && msg[8] == '\0')
        printf("please login first\n");

    else if(!strcmp(status, "OK ")) {
        memset(auctions, '\0', LST_PRINT*MAX_AUCTION+1);
        if (get_auctions(auctions, msg) == -1)
            return;
        printf("My Bids:\n%s", auctions);
    }
    else printf("%s", msg);
}

int get_fname_fsize(int fd, char *fname, long *fsize) {
    char aux;
    ssize_t nread;
    int offset = 0;
    int first_loop = 1;
    *fsize = 0;

    /* receive message from AS */
    while(1) {
        nread=read(fd,&aux,1); // read only one character
        if(nread == -1) {   //error
            printf("Can't receive from server AS. Try again\n");
            return -1;
        } else if(nread == 0) break; //closed by peer
              
        if (first_loop) {   /* receive fname */
            if (offset && aux == ' ') { // fname received
                first_loop = 0;
                offset = 0;
                continue;
            }
            if (!isdigit(aux) && !isalpha(aux) && aux != '-' && aux != '_' && aux != '.') {
                printf("fname not valid\n"); return -1; }
            if (offset >= FNAME) { printf("fname not valid\n"); return -1; }
            sprintf(fname+offset, "%c", aux);   // add char to fname
            offset++;
        } else {    /* receive fsize */
            if (offset && aux == ' ')   // fsize received
                break;
            if (!isdigit(aux)) { printf("fsize not valid\n"); return -1; }
            if (offset >= MAX_4_LONG) { printf("fsize not valid\n"); return -1; }
            *fsize = (*fsize)*10 + aux - '0';   // add digit to fsize
            offset++;
        }
    }
    return 0;
}

int process_sa(int fd, char *fname, char *msg) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    long fsize;

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3
    command[CMD_N_SPACE-1] = '\0';

    if(!strcmp(command, "RSA "))
        printf("%s", msg);
    
    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0') {
        printf("no file to be sent or other problem\n");
    }

    else if(!strcmp(status, "OK ") && msg[7] != ' ') {  
        if (get_fname_fsize(fd, fname, &fsize) == -1)
            return -1;
        if (receive_file(fd, fname, fsize) == -1)
            return -1;
        printf("%s file has been received and its size is %ld bytes\n", fname, fsize);
    }
    else printf("%s", msg);

    return 0;
}

void process_bid(char *msg, char *aid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3
    command[CMD_N_SPACE-1] = '\0';

    if(!strcmp(command, "RBD ") || msg[7] != '\n' || msg[8] != '\0')
        printf("%s", command);
    
    else if(!strcmp(status, "NOK")) {
        printf("auction %s is not active\n", aid);
    }

    else if(!strcmp(status, "NLG")) {
        printf("login is needed to bid on an auction\n");
    }

    else if(!strcmp(status, "ACC")) {
        printf("bid accepted\n");
    }

    else if(!strcmp(status, "REF")) {
        printf("bid refused because a larger bid is already been placed previously\n");
    }

    else if(!strcmp(status, "ILG")) {
        printf("user is not allowed to bid in an auction hosted by himself\n");
    }
    else printf("%s", msg);
}

long confirm_bid(char *msg, long initial, char *uid, long *value, char *date, int *bid_time) {
    char ints_to_str[MAX_4_LONG+1]; // max size of an long
    size_t offset = (size_t) initial;

    memset(uid, '\0', UID+1);
    memset(date, '\0', DATE_TIME+1);

    /* verify if the first letter is B and the second character is a space */
    if (msg[offset] != 'B' || msg[offset+1] != ' ' || msg[offset+2] == ' ')
        return 0;
    
    /* verify if bid_uid is valid and if theres is a space next */
    sscanf(msg+offset+2, "%6s", uid);
    offset += 2+UID+1;
    if(strlen(uid) != UID || !is_numeric(uid) || msg[offset-1] != ' ' || msg[offset] == ' ' || msg[offset] == '-')
        return 0;

    /* verify if bid_value is valid and if theres is a space next */
    if (sscanf(msg+offset, "%ld", value) != 1)
        return 0;
    
    sprintf(ints_to_str, "%ld", *value);
    offset += strlen(ints_to_str) + 1;
    if(msg[offset-1] != ' ' || msg[offset] == ' ')
        return 0;
    
    /* verify if bid_date-time is valid and if there is a space next */
    memcpy(date, msg+offset, DATE_TIME);
    offset += DATE_TIME+1;
    if (!isDateTime(date) || msg[offset-1] != ' ' || msg[offset] == ' ' || msg[offset] == '-')
        return 0;

    /* verify if bid_time is valid and if theres is a space next */
    if (sscanf(msg+offset, "%d", bid_time) != 1)
        return 0;
     
    sprintf(ints_to_str, "%d", *bid_time);
    offset += strlen(ints_to_str) + 1;
    if((msg[offset-1] != ' ' && msg[offset-1] != '\n' )|| msg[offset] == ' ')
        return 0;

    return (long) offset;
}

long get_bids_list(char *bids, char *msg, long offset) {
    char bid[BID+1], uid[UID+1], date[DATE_TIME+1];
    long value;
    int bid_time;

    memset(bid, '\0', BID+1);

    for (int i = 0; i < MAX_BIDS; i++) {
        /* get a bid from msg (BID) */
        offset = confirm_bid(msg, offset, uid, &value, date, &bid_time);
        if (offset == 0) {
            printf("%s", msg);
            return 0;
        }

        /* Append a bid to the message that will be shown to user */
        sprintf(bids + strlen(bids), "uid: %s\tbid_value: %ld\ttime_of_bid: %s\ttime_since_start: %d seconds\n",
                uid, value, date, bid_time);

        if (msg[offset] == 'E' || msg[offset-1] == '\n') break; // no more bids
    }
    return offset;
}

int get_bids(char *bids, char *msg, int initial) {
    char host_uid[UID+1], name[NAME+1], fname[FNAME+1], start_date[DATE_TIME+1], end_date[DATE_TIME+1];
    int start_value, timeactive;
    char ints_to_str[MAX_4_SOME_INTS+1];
    size_t offset = (size_t) initial;
    
    /* initialize strings with \0 in every index */
    memset(host_uid, '\0', UID+1);
    memset(name, '\0', NAME+1);
    memset(fname, '\0', FNAME+1);
    memset(start_date, '\0', DATE_TIME+1);
    memset(bids, '\0', SR_PRINT);
    memset(end_date, '\0', DATE_TIME+1);
    
    sscanf(msg+offset, "%6s", host_uid);
    offset += UID+1;    // advance string
    /* verify if the uid has the correct size and is only digits */

    if(strlen(host_uid) != UID || !is_numeric(host_uid) || msg[offset - 1] != ' ' || msg[offset] == ' ') {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    sscanf(msg+offset, "%10s", name);
    offset += strlen(name)+1;   // advance string
    /* verify if the string has the correct size is only letters and numbers and if spaces are placed correctly */
    if(strlen(name) > NAME || !is_alphanumeric_extra(name) || msg[offset-1] != ' ' || msg[offset] == ' ') {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    sscanf(msg+offset, "%24s", fname);
    offset += strlen(fname)+1;  // advance string
    /* verify if the string has the correct size is a valid file name and if spaces are placed correctly */
    if(strlen(fname) > FNAME || !is_alphanumeric_extra(name) || msg[offset-1] != ' ' || msg[offset] == ' ') {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* verify if the string is only digits */
    if (sscanf(msg+offset, "%d", &start_value) != 1) {
        printf("incorrect show_record attempt\n");
        return -1;
    }
    sprintf(ints_to_str, "%d", start_value);
    offset += strlen(ints_to_str)+1;
    /* verify if the spaces are placed correctly and max of 6 digits */
    if(msg[offset-1] != ' ' || msg[offset] == ' ' || msg[offset] == '-' || strlen(ints_to_str) > 6) {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* verify if the string is a date time */
    memcpy(start_date, msg+offset, DATE_TIME);
    offset += DATE_TIME+1;
    if (!isDateTime(start_date)) {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* verify if the string is only digits */
    if (sscanf(msg+offset, "%d", &timeactive) != 1) {
        printf("incorrect show_record attempt\n");
        return -1;
    }
    sprintf(ints_to_str, "%d", timeactive);
    offset += strlen(ints_to_str)+1;
    /* verify if the spaces are placed correctly and max of 5 digits */
    if((msg[offset-1] != ' ' || msg[offset] == ' ' || strlen(ints_to_str) > 5) && 
       (msg[offset-1] != '\n')) { //in case there are no bids and it's still active
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* First part of the message shown to user */
    sprintf(bids, "%s (%s) hosted by %s started with value %d, at %s. Will be open during %d seconds:\n",
            name, fname, host_uid, start_value, start_date, timeactive);

    if (msg[offset-1] == '\n') {
        sprintf(bids + strlen(bids), "No bids yet for this auction\nAuction is still active\n");
        return 0; // auction has no bids yet and is still active
    }

    /* verification of bids (B messages) */
    if(msg[offset] != 'E'){ // there are bids for this asset
        offset = (size_t) get_bids_list(bids, msg, (long) offset);
        if (offset == 0) {
            return -1;
        }
    } else
        sprintf(bids + strlen(bids), "No bids were made for this auction\n");


    if (msg[offset-1] == '\n') {
        sprintf(bids + strlen(bids), "Auction is still active\n");
        return 0;
    }        

    /* verify if the first letter is E and the second character is a space */
    if (msg[offset] != 'E' || msg[offset+1] != ' ' || msg[offset+2] == ' ') {
        printf("incorrect show_record attempt\n");
        return -1;
    }
    offset +=2;

    /* verify if the string is a date time */
    memcpy(end_date, msg+offset, DATE_TIME);
    offset += DATE_TIME+1;
    if (!isDateTime(start_date) || msg[offset-1] != ' ' || msg[offset] == ' ' || msg[offset] == '-') {
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* verify if the string is only digits */
    if (sscanf(msg+offset, "%d", &timeactive) != 1) {
        printf("incorrect show_record attempt\n");
        return -1;
    }
    sprintf(ints_to_str, "%d", timeactive);
    offset += strlen(ints_to_str);
    /* verify if the spaces are placed correctly and max of 5 digits */
    if(msg[offset] == ' ' || msg[offset+1] != '\0' || strlen(ints_to_str) > 5 || msg[offset] != '\n') { 
        printf("incorrect show_record attempt\n");
        return -1;
    }

    /* Last part of the message shown to user */
    sprintf(bids + strlen(bids), "Ended at %s, opened for %d seconds.\n", end_date, timeactive);

    return 0;
}

void process_sr(char *msg, char *aid) {
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char bids[SR_PRINT];

    get_cmd_status(msg, command, status);
    status[STATUS-1] = '\0'; // for next strcmp calls is needed strlen(status) = 3

    if(strcmp(command, "RRC "))
        printf("%s", msg);
    
    else if(!strcmp(status, "NOK") && msg[7] == '\n' && msg[8] == '\0')
        printf("%s does not exist\n", aid);

    else if(!strcmp(status, "OK ") && msg[7] != ' ') {
        if (get_bids(bids, msg, CMD_N_SPACE+STATUS-1) == -1)
            return;
        printf("Bids from auction %s - %s", aid, bids);
    }
    else printf("%s", msg);
}
