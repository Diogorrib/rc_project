#include "../common/constant.h"
#include "../common/utils.h"
#include "process_server.h"
#include "file_creation.h"

int read_password_file(const char *fname, char *existing_pass) {
    /* Open file for reading */
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        printf("ERR: Failed to open file %s for reading\n", fname);
        return -1;
    }
    /* Read the password from file */
    size_t bytesRead = fread(existing_pass, 1, PASSWORD, file);
    if(bytesRead < PASSWORD){
        printf("ERR: Failed to read the 8 characters of the password.\n");
        fclose(file); return -1;
    }
    // Add null terminator
    existing_pass[bytesRead] = '\0';
    fclose(file);
    return 0;
}

void process_login(const char *uid, const char *pass, char *msg) {
    char fname[64];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    sprintf(fname, "USERS/%s/%s_pass.txt", uid, uid);

    if(verify_file(fname)) {
        if (read_password_file(fname, existing_pass) == -1) {
            sprintf(msg, "ERR\n");
            return;
        }
        
        // if the uid_pass matches password: create login file
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
        sprintf(msg, "RLI NOK\n");
        return;
    }
    // if pass file does not exist: create login and password file
    if (!create_login(uid) || !create_password(uid,pass)) {
        sprintf(msg, "ERR\n");
        printf("ERR: Failed to create file");
        return;
    }
    sprintf(msg, "RLI REG\n");
}

void process_logout(const char *uid, const char *pass, char *msg) {
    char fname_login[64], fname_pass[64];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(fname_pass, "USERS/%s/%s_pass.txt", uid, uid);

    if(verify_file(fname_pass)) {
        if (read_password_file(fname_pass, existing_pass) == -1) {
            sprintf(msg, "ERR\n");
            return;
        }
        
        // if the uid_pass matches password: we can delete login file (if the file exists)
        if(!strcmp(existing_pass,pass)){
            if(verify_file(fname_login)){
                if(delete_file(fname_login) == -1){
                    sprintf(msg, "ERR\n");
                    printf("ERR: Failed to delete file");
                    return;
                }
                sprintf(msg, "RLO OK\n");
                return;
            }
            // if uid_pass does not match password or login file does not exist
            sprintf(msg, "RLO NOK\n");
            return;
        }
    }
    // if the pass file doesn't exist it means that the user is not registered
    sprintf(msg, "RLO UNR\n");
}

void process_unregister(const char *uid, const char *pass, char *msg) {
    char fname_pass[64];
    char fname_login[64];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    sprintf(fname_pass, "USERS/%s/%s_pass.txt", uid, uid);
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);

    if(verify_file(fname_pass)) {
        if (read_password_file(fname_pass, existing_pass) == -1) {
            sprintf(msg, "ERR\n");
            return;
        }
        
        // if the uid_pass matches password: we can delete login file and pass file (if the file exists)
        if(!strcmp(existing_pass,pass)){
            if(verify_file(fname_login)){
                if((delete_file(fname_login) == -1) || (delete_file(fname_pass) == -1)){
                    sprintf(msg, "ERR\n");
                    printf("ERR: Failed to delete file");
                    return;
                }
                sprintf(msg, "RUR OK\n");
                return;
            }
            // if uid_pass does not match password or login file does not exist
            sprintf(msg, "RUR NOK\n");
            return;
        }
    }
    // if the pass file doesn't exist it means that the user is not registered
    sprintf(msg, "RUR UNR\n");
}
