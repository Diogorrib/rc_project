#include "../common/constant.h"
#include "../common/utils.h"
#include "process_server.h"
#include "file_creation.h"

void process_login(const char *uid, const char *pass, char *msg) {
    char fname[64];
    char existing_pass[9]; // 8 letters plus '\0' to terminate the string
    sprintf(fname, "USERS/%s/%s_pass.txt", uid, uid);

    if(verify_file(fname)) {
        /* Open file for reading */
        FILE *file = fopen(fname, "rb");
        if (file == NULL) {
            sprintf(msg, "ERR\n");
            printf("ERR: Failed to open file %s for reading\n", fname);
            return;
        }
        size_t bytesRead = fread(existing_pass, 1, 8, file);
        if( bytesRead < 8){
            sprintf(msg, "ERR\n");
            printf("ERR: Failed to read the eigth characters of the password.\n");
            return;
        }
        // Add null terminator
        existing_pass[bytesRead] = '\0';
        // if the uid_pass matches password && create login file
        if(!strcmp(existing_pass,pass)){
            if (!create_login(uid)) {
                sprintf(msg, "ERR\n");
                printf("ERR: Failed to create file");
                return;
            }
            sprintf(msg, "RLI OK\n");
            return;
        }
        // if uid_pass does not match password
        if(strcmp(existing_pass,pass)){
            sprintf(msg, "RLI NOK\n");
            return;
        }
    }

    else{
        if (!create_login(uid) || !create_password(uid,pass)) {
            sprintf(msg, "ERR\n");
            printf("ERR: Failed to create file");
            return;
        }
        sprintf(msg, "RLI REG\n");
        return;
    }
}