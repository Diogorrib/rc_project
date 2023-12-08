#include "../common/constant.h"
#include "../common/utils.h"
#include "file_creation.h"

/// @brief Convert a time_t value to a date time string (YYYY-MM-DD HH:MM:SS)
/// @param fulltime 
/// @param converted_date 
void convert_to_date(time_t fulltime, char *converted_date) {
    struct tm *current_time;
    char time_str[DATE_TIME+17]; // 17 because an error occurs when using just DATE_TIME+1 (the size required)
    current_time = gmtime(&fulltime); // Convert time to YYYY-MM-DD HH:MM:SS
    sprintf(time_str ,"%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    memcpy(converted_date, time_str, DATE_TIME);
    converted_date[DATE_TIME] = '\0';
}

////////////////////////////////////// USERS DIRECTORY FILES //////////////////////////////////////////////////////////

int create_login(const char *uid) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    char fdata[12];
    
    sprintf(dirname, "USERS/%s", uid);
    sprintf(filepath, "%s/%s_login.txt", dirname, uid);
    sprintf(fdata, "Logged in\n");
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_password(const char *uid, const char *pass) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    char fdata[PASSWORD+2];
    
    sprintf(dirname, "USERS/%s", uid);
    sprintf(filepath, "%s/%s_pass.txt", dirname, uid);
    sprintf(fdata, "%s\n", pass);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_hosted(const char *uid, const char *aid){
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    sprintf(dirname, "USERS/%s/HOSTED", uid);
    sprintf(filepath, "%s/%s.txt", dirname, aid);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

int create_bidded(const char *uid, const char *aid){
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    sprintf(dirname, "USERS/%s/BIDDED", uid);
    sprintf(filepath, "%s/%s.txt", dirname, aid);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

////////////////////////////////////// AUCTIONS DIRECTORY FILES ///////////////////////////////////////////////////////

int create_start(const char *aid, const char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];

    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/START_%s.txt", dirname, aid);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_asset(int fd, const char *aid, const char *fname, const char *fsize) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    sprintf(dirname, "AUCTIONS/%s",aid);
    sprintf(filepath, "%s/%s", dirname, fname);
    long size = (long) atoi(fsize);
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    if (receive_file(fd, filepath, size, SERVER_TCP_TIMEOUT) == -1) {
        if (rmdir(dirname) != 0)
            printf("Directory %s not deleted.\n", dirname);
        return 0;
    }
    return 1;
}

int create_end(const char *aid, char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];

    sprintf(dirname, "AUCTIONS/%s", aid);
    sprintf(filepath, "%s/END_%s.txt", dirname, aid);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_bid_value(const char *aid, int bid_value, char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    sprintf(dirname, "AUCTIONS/%s/BIDS", aid);
    sprintf(filepath, "%s/%06d.txt", dirname, bid_value);
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

//////////////////////////////////// MULTIPLE FILE CREATION OR CONDITIONAL ////////////////////////////////////////////

int create_open_files(const char *aid, const char *uid, char *fdata) {
    time_t fulltime;
    char time_str[DATE_TIME+1];

    time(&fulltime); // Get current time in seconds starting at 1970
    convert_to_date(fulltime, time_str);

    sprintf(fdata+strlen(fdata), "%s %ld\n", time_str, fulltime);
    if (!create_hosted(uid, aid) || !create_start(aid, fdata))
        return 0;
    return 1;
}

int create_end_if_expired(const char *aid, int timeactive, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time;
    time_t limit_time = (time_t) (starttime + (long) timeactive);
    char time_str[DATE_TIME+1];
    
    time(&actual_time);
    if (actual_time >= limit_time) {
        convert_to_date(limit_time, time_str);
        sprintf(fdata, "%s %d\n", time_str, timeactive);
        if (!create_end(aid, fdata))
            return 0;
    }
    return 1;
}

int create_end_close(const char *aid, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time;
    char time_str[DATE_TIME+1];
    
    time(&actual_time);
    convert_to_date(actual_time, time_str);
    int timeactive = (int) (actual_time - (time_t) starttime);
    sprintf(fdata, "%s %d\n", time_str, timeactive);
    if (!create_end(aid, fdata))
        return 0;
    return 1;
}

int create_bid_files(const char *uid, const char *aid, const char *value, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time, bid_sec_time;
    char time_str[DATE_TIME+1];
    int bid_value = atoi(value);
    
    time(&actual_time);
    bid_sec_time = (time_t) (actual_time - (time_t) starttime);
    convert_to_date(actual_time, time_str);
    sprintf(fdata, "%s %s %s %ld\n", uid, value, time_str, bid_sec_time);

    if (!create_bid_value(aid, bid_value, fdata) || !create_bidded(uid, aid))
        return 0;
    return 1;
}
