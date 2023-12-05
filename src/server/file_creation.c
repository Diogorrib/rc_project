#include "../common/constant.h"
#include "../common/utils.h"
#include "file_creation.h"

void convert_to_date(time_t fulltime, char *converted_date) {
    struct tm *current_time;
    current_time = gmtime(&fulltime); // Convert time to YYYY-MM-DD HH:MM:SS
    sprintf(converted_date ,"%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
}

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

int create_asset(int fd, const char *aid, const char *fname, const char *fsize) {
    char filepath[64];
    char dirname[20];
    
    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/%s",dirname,fname);
    long size = (long) atoi(fsize);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    if (receive_file(fd, filepath, size) == -1) {
        if (rmdir(dirname) != 0)
            printf("Directory %s not deleted.\n", dirname);
        return 0;
    }
    
    return 1;
}

/* verifica se excedeu o tempo e cria o ficheiro */
int create_end(const char *aid, int timeactive, long starttime) {
    char endfile[64];
    char dirname[20];
    char fdata[BUFFER_512];
    time_t actual_time;
    time_t limit_time = (time_t) (starttime + (long)timeactive);
    char time_str[DATE_TIME+17]; // 17 because an error occurs when using just DATE_TIME+1 (the size required)
    
    time(&actual_time);
    if (actual_time >= limit_time) {
        convert_to_date(limit_time, time_str);
        memset(fdata, '\0', BUFFER_512);
        sprintf(fdata, "%s %d\n", time_str, timeactive);
        sprintf(dirname, "AUCTIONS/%s",aid);
        sprintf(endfile, "%s/END_%s.txt",dirname,aid);
        if (create_file(endfile, dirname, fdata) == -1)
            return 0;
    }
    return 1;
}

int create_end_close(const char *aid, long starttime) {
    char endfile[64];
    char dirname[20];
    char fdata[BUFFER_512];
    time_t actual_time;
    char time_str[DATE_TIME+17]; // 17 because an error occurs when using just DATE_TIME+1 (the size required)
    
    time(&actual_time);
    convert_to_date(actual_time, time_str);
    int timeactive = (int) (actual_time - (time_t) starttime);
    memset(fdata, '\0', BUFFER_512);
    sprintf(fdata, "%s %d\n", time_str, timeactive);
    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(endfile, "%s/END_%s.txt",dirname,aid);
    if (create_file(endfile, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_bid_value(const char *uid, const char *aid, const char *value, long starttime) {
    char fname_auctions[64], fname_users[64];
    char dirname_auctions[20], dirname_users[20];
    char fdata[BUFFER_512];
    time_t actual_time;
    char time_str[DATE_TIME+17]; // 17 because an error occurs when using just DATE_TIME+1 (the size required)
    int bid_value = atoi(value);
    
    time(&actual_time);
    time_t bid_sec_time = (time_t) (actual_time - (time_t)starttime);
    convert_to_date(actual_time, time_str);
    sprintf(fdata, "%s %s %s %ld\n", uid, value, time_str, bid_sec_time);

    sprintf(dirname_auctions, "AUCTIONS/%s/BIDS",aid);
    sprintf(fname_auctions, "%s/%06d.txt",dirname_auctions,bid_value);
    if (create_file(fname_auctions, dirname_auctions, fdata) == -1)
        return 0;

    sprintf(dirname_users, "USERS/%s/BIDDED",uid);
    sprintf(fname_users, "%s/%s.txt",dirname_users,aid);
    if (create_file(fname_users, dirname_users, NULL) == -1)
        return 0;
    return 1;
}

int create_open_files(const char *aid, const char *uid, char *fdata) {
    time_t fulltime;
    char time_str[DATE_TIME+17]; // 17 because an error occurs when using just DATE_TIME+1 (the size required)

    time(&fulltime); // Get current time in seconds starting at 1970
    convert_to_date(fulltime, time_str);

    sprintf(fdata+strlen(fdata), "%s %ld\n", time_str, fulltime);
    if (!create_hosted(uid, aid) || !create_start(aid, fdata))
        return 0;
    return 1;
}
