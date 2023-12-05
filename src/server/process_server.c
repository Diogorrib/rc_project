#include "../common/constant.h"
#include "../common/utils.h"
#include "process_server.h"
#include "file_creation.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

int read_start_file(const char *path, char *uid, char *name, char *fname, int *start_value,
                    int *timeactive, char *datetime, long *starttime) {
    char aux_uid[DATE_TIME+1], aux_name[NAME+1], aux_fname[FNAME+1];
    int aux_value, aux_active;
    char date[DATE+1], hour[HOUR+1];
    long aux_time;

    /* Open file for reading */
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("ERR: Failed to open file %s for reading\n", path);
        return -1;
    }
    int result = fscanf(file, "%s %s %s %d %d %s %s %ld",
            aux_uid, aux_name, aux_fname, &aux_value, &aux_active, date, hour, &aux_time);
    if(result != 8) {
        printf("ERR: Failed to read file.\n");
        fclose(file); return -1;
    }
    fclose(file);

    if (uid != NULL)
        sprintf(uid, "%s", aux_uid);
    if (name != NULL)
        sprintf(name, "%s", aux_name);
    if (fname != NULL)
        sprintf(fname, "%s", aux_fname);
    if (start_value != NULL)
        *start_value = aux_value;
    if (timeactive != NULL)
        *timeactive = aux_active;
    if (datetime != NULL)
        sprintf(datetime, "%s %s", date, hour);
    if (starttime != NULL)
        *starttime = aux_time;
    return 0;
}

int read_start(const char *fname_start, char *msg) {
    char host[DATE_TIME+1], name[NAME+1], fname[FNAME+1];
    int start_value, timeactive;
    char datetime[DATE_TIME+1];

    read_start_file(fname_start, host, name, fname, &start_value, &timeactive, datetime, NULL);
    sprintf(msg + strlen(msg),"%s %s %s %d %s %d",
        host, name, fname, start_value, datetime, timeactive);
    return 0;
}

int read_end(const char *fname_end, char *msg) {
    char date[DATE+1];
    char hour[HOUR+1];
    int end_sec_time;

    /* Open file for reading */
    FILE *file = fopen(fname_end, "rb");
    if (file == NULL) {
        printf("ERR: Failed to open file %s for reading\n", fname_end);
        return -1;
    }
    int result = fscanf(file, "%s %s %d", date, hour, &end_sec_time);
    if(result != 3) {
        printf("ERR: Failed to read file.\n");
        fclose(file); return -1;
    }
    fclose(file);

    sprintf(msg + strlen(msg)," E %s %s %d", date, hour, end_sec_time);
    return 0;
}

void verify_auction_end() {
    char filepath[64], dirname[20], aid[AID+1];
    int timeactive;
    long starttime;
    for (int i = 1; i <= MAX_AUCTION; i++) {
        memset(aid, '\0', AID+1);
        sprintf(aid, "%03d", i);
        memset(dirname, '\0', 20);
        sprintf(dirname, "AUCTIONS/%s", aid);
        if(!verify_directory(dirname))  // auction does not exist
            break;

        memset(filepath, '\0', 64);
        sprintf(filepath, "AUCTIONS/%s/END_%s.txt", aid, aid);
        if (!verify_file(filepath)) {   // auction is active
            memset(filepath, '\0', 64);
            sprintf(filepath, "AUCTIONS/%s/START_%s.txt", aid, aid);
            if (read_start_file(filepath, NULL, NULL, NULL, NULL, &timeactive, NULL, &starttime) != -1) {
                create_end(aid, timeactive, starttime);
            }
        }
    }
}

void get_auctions(const char *dirname, const char *cmd, char *msg) {
    struct dirent **filelist;
    int n_entries;
    char fname_auction[64];
    char aid[AID+1];
    int has_a_file = 0;
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) {// Could test for -1 since n_entries count always with . and ..
        sprintf(msg, "%s NOK\n", cmd);
        return;
    }

    for (int i = 0; i < n_entries; i++) {
        if (strlen(filelist[i]->d_name) == AID+4) { // Discard '.' , '..' and invalid filenames by size
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

void get_highest_bid(const char *dirname, const char *start_file, char *bid_value) {
    struct dirent **filelist;
    int n_entries;
    int has_a_file = 0;
    int start_value;

    read_start_file(start_file, NULL, NULL, NULL, &start_value, NULL, NULL, NULL);

    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) {// Could test for -1 since n_entries count always with . and ..
        sprintf(bid_value, "%06d", start_value);
        return;
    }

    while (n_entries--) {
        if (strlen(filelist[n_entries]->d_name) == MAX_4_SOME_INTS+4) { // Discard '.' , '..' and invalid filenames by size
            if (!has_a_file) {
                memset(bid_value, '\0', MAX_4_SOME_INTS+1);
                memcpy(bid_value, filelist[n_entries]->d_name, MAX_4_SOME_INTS);    // get value from file name
                has_a_file = 1;
            }
        }
        free(filelist[n_entries]);
    }
    free(filelist);
    if(!has_a_file) // vai buscar ao start
        sprintf(bid_value, "%06d", start_value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// PROCESS FUNCTIONS /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void process_ma(const char *uid, char *msg) {
    char fname_login[64];
    char hosted_dir[20];
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(hosted_dir, "USERS/%s/HOSTED", uid);

    if(verify_file(fname_login)) {
        if(verify_directory(hosted_dir)) {
            get_auctions(hosted_dir, "RMA", msg);
            return;
        }
        // HOSTED dir does not exist (user has no auctions)
        sprintf(msg, "RMA NOK\n");
        return;
    }
    // if login file does not exist
    sprintf(msg, "RMA NLG\n");
}

void process_mb(const char *uid, char *msg) {
    char fname_login[64];
    char bidded_dir[20];
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(bidded_dir, "USERS/%s/BIDDED", uid);

    if(verify_file(fname_login)) {
        if(verify_directory(bidded_dir)) {
            get_auctions(bidded_dir, "RMB", msg);
            return;
        }
        // BIDDED dir does not exist (user has no bids)
        sprintf(msg, "RMB NOK\n");
        return;
    }
    // if login file does not exist
    sprintf(msg, "RMB NLG\n");
}

void get_bids(const char *dirname, char *msg) {
    struct dirent **filelist;
    int n_entries;
    char fname_bid[512];
    char uid[UID+1], date[DATE+1], hour[HOUR+1];
    int bid_value, bid_sec_time;
    n_entries = scandir(dirname, &filelist, 0, alphasort);
    if (n_entries <= 0) {// Could test for -1 since n_entries count always with . and ..
        return;
    }
    int count = 0;
    while(n_entries--) {
        if (count < MAX_BIDS && strlen(filelist[n_entries]->d_name) == MAX_4_SOME_INTS+4) { // Discard '.' , '..' and invalid filenames by size
            memset(fname_bid, '\0', 512);
            sprintf(fname_bid, "%s/%s", dirname, filelist[n_entries]->d_name);
            /* Open file for reading */
            FILE *file = fopen(fname_bid, "rb");
            if (file == NULL) {
                printf("ERR: Failed to open file %s for reading\n", fname_bid);
                return;
            }
            int result = fscanf(file, "%s %d %s %s %d", uid, &bid_value, date, hour, &bid_sec_time);
            if(result != 5) {
                printf("ERR: Failed to read file.\n");
                fclose(file); return;
            }
            fclose(file);
            
            sprintf(msg + strlen(msg)," B %s %d %s %s %d", uid, bid_value, date, hour, bid_sec_time);
            count++;
        }
        free(filelist[n_entries]);
    }
    free(filelist);
}

void process_sr(const char *aid, char *msg) {
    char fname_start[64];
    char fname_end[64];
    char dirname[20];
    char bidded_dir[20];
    sprintf(dirname, "AUCTIONS/%s", aid);
    sprintf(bidded_dir, "AUCTIONS/%s/BIDS", aid);
    sprintf(fname_start,"AUCTIONS/%s/START_%s.txt", aid, aid);
    sprintf(fname_end, "AUCTIONS/%s/END_%s.txt", aid, aid);

    if(verify_directory(dirname)) {
        sprintf(msg, "RRC OK ");
        read_start(fname_start, msg);
        if(verify_directory(bidded_dir)) {
            get_bids(bidded_dir, msg);
        }
        // acresecntar end se existir
        if(verify_file(fname_end)) {
            read_end(fname_end, msg);
        }
        sprintf(msg + strlen(msg), "\n");
        return;
    }
    // if auction does not exist
    sprintf(msg, "RRC NOK\n");
}

int process_sa(const char *aid, char *fname, long *fsize, char *msg) {
    char dirname[20];
    char filepath[64];
    char fname_start[64];
    sprintf(dirname, "AUCTIONS/%s", aid);
    sprintf(fname_start,"AUCTIONS/%s/START_%s.txt", aid, aid);

    if(verify_directory(dirname)) {
        read_start_file(fname_start, NULL, NULL, fname, NULL, NULL, NULL, NULL);
        sprintf(filepath,"AUCTIONS/%s/%s", aid, fname);
        if (get_file_size(filepath, fsize) == -1) {
            sprintf(msg, "RSA NOK\n");
            return -1;
        }
        return 0;
    }
    // if auction does not exist
    sprintf(msg, "RSA NOK\n");
    return -1;
}

void process_list(char *msg) {
    char fname_auction[64];
    char dirname[20];
    int has_a_file = 0;
    for (int i = 1; i <= MAX_AUCTION; i++) {
        memset(dirname, '\0', 20);
        sprintf(dirname, "AUCTIONS/%03d", i);
        if(!verify_directory(dirname))  // auction does not exist
            break;

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
    sprintf(msg + strlen(msg), "\n");
}

void process_close(const char *uid, const char *pass, const char *aid, char *buffer) {
    char fname_pass[64];
    char fname_login[64];
    char fname_auction[64];
    char fname_start[64];
    char fname_hosted[64];
    char aid_dirname[20];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    int timeactive;
    long starttime;
    
    sprintf(fname_pass, "USERS/%s/%s_pass.txt", uid, uid);
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(aid_dirname, "AUCTIONS/%s", aid);
    sprintf(fname_auction, "AUCTIONS/%s/END_%s.txt", aid, aid);
    sprintf(fname_hosted, "USERS/%s/HOSTED/%s.txt", uid, aid);
    sprintf(fname_start, "AUCTIONS/%s/START_%s.txt", aid, aid);
    
    if(verify_file(fname_pass)){
        if (read_password_file(fname_pass, existing_pass) == -1 || strcmp(existing_pass,pass)) {
            sprintf(buffer, "ERR\n");
            return;
        }

        if(!verify_file(fname_login)) { // if login file does not exist
            sprintf(buffer, "RCL NLG\n");
            return;
        }

        if (!verify_directory(aid_dirname)) { // if the auction does not exist
            sprintf(buffer, "RCL EAU\n");
            return;
        }

        if (!verify_file(fname_hosted)) { // auction not hosted by himself
            sprintf(buffer, "RCL EOW\n");
            return;
        }
        
        if(verify_file(fname_auction)) { // auction not active
            sprintf(buffer, "RCL END\n");
            return;
        }
        
        if (read_start_file(fname_start, NULL, NULL, NULL, NULL, &timeactive, NULL, &starttime) != -1) {
            create_end_close(aid, starttime);
            sprintf(buffer, "RCL OK\n");
            return;
        }
    }
    sprintf(buffer, "ERR\n");
}

void process_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer) {
    char fname_pass[64];
    char fname_login[64];
    char fname_auction[64];
    char fname_start[64];
    char fname_hosted[64];
    char aid_dirname[20];
    char bid_dirname[20];
    char highest_bid[MAX_4_SOME_INTS+1];
    char existing_pass[PASSWORD+1]; // 8 letters plus '\0' to terminate the string
    int timeactive;
    long starttime;
    
    sprintf(fname_pass, "USERS/%s/%s_pass.txt", uid, uid);
    sprintf(fname_login, "USERS/%s/%s_login.txt", uid, uid);
    sprintf(aid_dirname, "AUCTIONS/%s", aid);
    sprintf(bid_dirname, "AUCTIONS/%s/BIDS", aid);
    sprintf(fname_auction, "AUCTIONS/%s/END_%s.txt", aid, aid);
    sprintf(fname_hosted, "USERS/%s/HOSTED/%s.txt", uid, aid);
    sprintf(fname_start, "AUCTIONS/%s/START_%s.txt", aid, aid);
    
    if(verify_file(fname_pass) && verify_directory(aid_dirname)){
        if (read_password_file(fname_pass, existing_pass) == -1 || strcmp(existing_pass,pass)) {
            sprintf(buffer, "ERR\n");
            return;
        }
        
        if(verify_file(fname_auction)) { // auction not active
            sprintf(buffer, "RBD NOK\n");
            return;
        }
        if(!verify_file(fname_login)) { // if login file does not exist
            sprintf(buffer, "RBD NLG\n");
            return;
        }

        get_highest_bid(bid_dirname, fname_start, highest_bid);
        int int_highest_bid = atoi(highest_bid);
        int int_bid_value = atoi(bid_value);

        if(int_bid_value <= int_highest_bid){ // a larger bid has already been placed previously
            sprintf(buffer, "RBD REF\n");
            return;
        }
        
        if (verify_file(fname_hosted)) { // bid in an auction hosted by himself
            sprintf(buffer, "RBD ILG\n");
            return;
        }
        
        if (read_start_file(fname_start, NULL, NULL, NULL, NULL, &timeactive, NULL, &starttime) != -1) {
            create_bid_value(uid, aid, bid_value, starttime);
            sprintf(buffer, "RBD ACC\n");
            return;
        }
    }
    sprintf(buffer, "ERR\n");
}