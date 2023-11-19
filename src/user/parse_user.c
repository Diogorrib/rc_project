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
    (void)start_value;
    (void)timeactive;
    (void)cmd_size;

    fname = (char *)malloc(fname_size);
    memset(name, '\0', NAME+1); // initialize the name with \0 in every index
    memset(fname, '\0', fname_size); // initialize the fname with \0 in every index

    // TODO
    return NULL;
}