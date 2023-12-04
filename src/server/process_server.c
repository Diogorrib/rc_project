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
            
        }
        // if uid_pass does not match password or login file does not exist
        sprintf(msg, "RLO NOK\n");
        return;
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
        }
        // if uid_pass does not match password or login file does not exist
        sprintf(msg, "RUR NOK\n");
        return;
    }
    // if the pass file doesn't exist it means that the user is not registered
    sprintf(msg, "RUR UNR\n");
}

int process_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *aid, char *buffer) {
    char fname_pass[64];
    char fname_login[64];
    char fname_asset[64];
    char dirname[20];
    char fdata[1024];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    sprintf(fname_pass, "USERS/%s/%s_pass.txt", uid, uid);
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(fname_asset, "AUCTIONS/%s/%s", aid, fname);
    sprintf(dirname, "AUCTIONS/%s", aid);

    if(verify_file(fname_pass)) {
        if (read_password_file(fname_pass, existing_pass) == -1) {
            sprintf(buffer, "ERR\n");
            return -1;
        }
        
        // if the uid_pass matches password: we can delete login file and pass file (if the file exists)
        if(!strcmp(existing_pass,pass)) {
            if(verify_file(fname_login)){ // if user is logged in
                sprintf(fdata, "%s %s %s %s %s ", uid, name, fname, start_value, timeactive);
                if (!create_open_files(aid, uid, fdata)) {
                    sprintf(buffer, "ERR\n");
                    return -1;
                }
                sprintf(buffer, "ROA OK %s\n", aid);
                return 0;
            }
            // if login file does not exist
            delete_file(fname_asset); rmdir(dirname);
            sprintf(buffer, "ROA NLG\n");
            return -1;
        }
    }
    // if the pass file doesn't exist or uid_pass does not match password
    delete_file(fname_asset); rmdir(dirname);
    sprintf(buffer, "ROA NOK\n");
    return -1;
}
/* 
int filter(const struct dirent *entry) {
    // Filter function to include only directories
    return (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0);
} */

void get_auctions(const char *dirname, const char *cmd, char *msg, int size) {
    struct dirent **filelist;
    int n_entries;
    char fname_auction[64];
    char aid[AID+1];
    int has_a_file = 0;
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) {// Could test for -1 since n_entries count always with . and ..
        sprintf(msg, "%s NOK\n", cmd);
        return;
    } printf("AAAAAAAA\n");

    for (int i = 0; i < n_entries; i++) {
        printf("%s\n", filelist[i]->d_name);
        if (strlen(filelist[i]->d_name) == size) { // Discard '.' , '..' and invalid filenames by size
            if (!has_a_file) {
                has_a_file = 1;
                sprintf(msg, "%s OK", cmd);
            }
            memset(aid, '\0', AID+1);
            memset(fname_auction, '\0', 64);
            memcpy(aid, filelist[i]->d_name, AID);    // get aid from file name
            sprintf(fname_auction, "AUCTIONS/%s/END_%s.txt", aid, aid);
            sprintf(msg + strlen(msg), " %s", aid);
            if (verify_file(fname_auction)) // auction ended
                sprintf(msg + strlen(msg), " 0");
            else                            // auction active
                sprintf(msg + strlen(msg), " 1");
        }
        free(filelist[i]);
    }
    if(!has_a_file){ // dir exists but is empty (user has no auctions / bids)
        sprintf(msg, "%s NOK\n", cmd);
        return;
    }
    sprintf(msg + strlen(msg), "\n");
    free(filelist);
}

/* int get_auctions(const char *dirname, const char *cmd, char *msg) {
    struct dirent **filelist;
    int n_entries, len;
    char dirname[20];
    char pathname[64];
    char aid[AID+1];
    sprintf(dirname, "AUCTIONS", AID) ;
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0)// Could test for -1 since n_entries count always with . and . .
        return -1;
    list->no_bids=0;
    while(n_entries--) {
        len = strlen(filelist[n_entries]->d_name) ;
        if (len == 7) { // Discard '.' , '..' and invalid filenames by size
            memset(aid, '\0', AID+1);
            memset(pathname, '\0', 64);
            memcpy(aid, filelist[n_entries]->d_name, AID);    // get aid from file name
            sprintf(pathname, "AUCTIONS/%s/END_%s.txt", aid, aid);
        }
        free ( filelist[n_entries] ) ;
        if (n_bids==50) {
            break;
        }
    }
    free(filelist);
    return 0;
} */
void process_ma(const char *uid, char *msg) {
    char fname_login[64];
    char hosted_dir[20];
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(hosted_dir, "USERS/%s/HOSTED", uid);

    if(verify_file(fname_login)) {
        if(verify_directory(hosted_dir)) {
            get_auctions(hosted_dir, "RMA", msg, 7);
            return;
        }
        // HOSTED dir does not exist (user has no auctions)
        sprintf(msg, "RMA NOK\n");
        return;
    }
    // if login file does not exist
    sprintf(msg, "RMA NLG\n");
}

    /* char fname_auction[64];
    int has_a_file = 0;
    for (int i = 1; i < actual_aid; i++) {
        if (!has_a_file) {
            has_a_file = 1;
            sprintf(msg, "RLS OK");
        }
        memset(fname_auction, '\0', 64);
        sprintf(fname_auction, "AUCTIONS/%03d/END_%03d.txt", i, i);
        sprintf(msg + strlen(msg), " %03d", i);
        if (verify_file(fname_auction)) // auction ended
            sprintf(msg + strlen(msg), " 0");
        else                            // auction active
            sprintf(msg + strlen(msg), " 1");
    }
    if(!has_a_file) { // no auctions
        sprintf(msg, "RLS NOK\n");
        return;
    }
    sprintf(msg + strlen(msg), "\n"); */
