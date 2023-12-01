#include "../common/constant.h"
#include "../common/utils.h"
#include "file_creation.h"

int create_login(const char *uid) {
    char filepath[64];
    char dirname[20];
    char fdata[512];
    
    sprintf(dirname, "USERS/%s",uid);
    sprintf(filepath, "%s/%s_login.txt",dirname,uid);
    sprintf(fdata, "Logged in\n");
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_password(const char *uid, const char *pass) {
    char filepath[64];
    char dirname[20];
    char fdata[16];
    
    sprintf(dirname, "USERS/%s",uid);
    sprintf(filepath, "%s/%s_pass.txt",dirname,uid);
    sprintf(fdata, "%s\n", pass);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_hosted(const char *uid, const char *aid){
    char filepath[64];
    char dirname[20];
    
    sprintf(dirname, "USERS/%s/HOSTED", uid);
    sprintf(filepath, "%s/%s.txt",dirname,aid);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

int create_bidded(const char *uid, const char *aid){
    char filepath[64];
    char dirname[20];
    
    sprintf(dirname, "USERS/%s/BIDDED", uid);
    sprintf(filepath, "%s/%s.txt",dirname,aid);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

int create_start(const char *aid, const char *fdata) {
    char filepath[64];
    char dirname[20];

    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/START_%s.txt",dirname,aid);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_asset(const char *aid, const char *fname, const char *fdata) {
    char filepath[64];
    char dirname[20];
    
    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/%s",dirname,fname);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_end(const char *aid, const char *fdata) {
    char filepath[64];
    char dirname[20];

    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/END_%s.txt",dirname,aid);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_bid_value(const char *aid, const char *value, const char *fdata) {
    char filepath[64];
    char dirname[20];

    sprintf(dirname, "AUCTIONS/%s/BIDS",aid);
    sprintf(filepath, "%s/%s.txt",dirname,value);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}
