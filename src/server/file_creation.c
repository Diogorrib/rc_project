#include "../common/constant.h"
#include "../common/utils.h"
#include "file_creation.h"

/// @brief Convert a time_t value to a date time string (YYYY-MM-DD HH:MM:SS)
/// @param fulltime The time in seconds since the Epoch
/// @param converted_date A pointer to the character array to store the converted date
void convert_to_date(time_t fulltime, char *converted_date) {
    struct tm *current_time;          // structure to hold broken-down time information
    char time_str[DATE_TIME+17];      // 17 because an error occurs when using just DATE_TIME+1 (the size required)
    current_time = gmtime(&fulltime); // convert time to YYYY-MM-DD HH:MM:SS
    
    /* format the date and time into a string: "YYYY-MM-DD HH:MM:SS */
    sprintf(time_str ,"%4d-%02d-%02d %02d:%02d:%02d",
            current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    
    /* copy the formatted string to the provided buffer
    (using DATE_TIME to ensure the correct number of characters are copied) */
    memcpy(converted_date, time_str, DATE_TIME);
    
    /* add null-terminator to ensure the string is properly terminated */
    converted_date[DATE_TIME] = '\0';
}

////////////////////////////////////// USERS DIRECTORY FILES //////////////////////////////////////////////////////////

int create_login(const char *uid) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    char fdata[12];
    
     /* construct the directory path for the user */
    sprintf(dirname, "USERS/%s", uid);
    /* construct the filepath for the login file */
    sprintf(filepath, "%s/%s_login.txt", dirname, uid);
    /* set the login message data */
    sprintf(fdata, "Logged in\n");
    /* create the login file using the provided utility function create_file */
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_password(const char *uid, const char *pass) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    char fdata[PASSWORD+2];
    
     /* construct the directory path for the user */
    sprintf(dirname, "USERS/%s", uid);
    /* construct the filepath for the password file */
    sprintf(filepath, "%s/%s_pass.txt", dirname, uid);
    /* set the password message data */
    sprintf(fdata, "%s\n", pass);
    /* create the password file using the provided utility function create_file */
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_hosted(const char *uid, const char *aid){
    char filepath[FILEPATH];
    char dirname[DIRNAME];

     /* construct the directory path for the user */
    sprintf(dirname, "USERS/%s/HOSTED", uid);
    /* construct the filepath for the hosted file */
    sprintf(filepath, "%s/%s.txt", dirname, aid);
    /* create the hosted file using the provided utility function create_file */
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

int create_bidded(const char *uid, const char *aid){
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
     /* construct the directory path for the user */
    sprintf(dirname, "USERS/%s/BIDDED", uid);
    /* construct the filepath for the bidded file */
    sprintf(filepath, "%s/%s.txt", dirname, aid);
    /* create the bidded file using the provided utility function create_file */
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;
    return 1;
}

////////////////////////////////////// AUCTIONS DIRECTORY FILES ///////////////////////////////////////////////////////

int create_start(const char *aid, const char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];

     /* construct the directory path for the auction */
    sprintf(dirname, "AUCTIONS/%s",aid);
    /* construct the filepath for the start file */
    sprintf(filepath, "%s/START_%s.txt", dirname, aid);
    /* create the start file using the provided utility function create_file */
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_asset(int fd, const char *aid, const char *fname, const char *fsize) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    /* construct the directory path for the auction */
    sprintf(dirname, "AUCTIONS/%s",aid);
    /* construct the filepath for the asset file */
    sprintf(filepath, "%s/%s", dirname, fname);
    /* convert the string representation of file size to a long */
    long size = (long)atoi(fsize);
    /* create the asset file using the provided utility function create_file */
    if (create_file(filepath, dirname, NULL) == -1)
        return 0;  
    /* receive the asset file from the communication channel */
    if (receive_file(fd, filepath, size, SERVER_TCP_TIMEOUT) == -1) {
        /* if receiving fails, delete the directory */
        if (rmdir(dirname) != 0)
            printf("Directory %s not deleted.\n", dirname);
        return 0;
    }
    return 1;
}

int create_end(const char *aid, char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];

    /* construct the directory path for the auction */
    sprintf(dirname, "AUCTIONS/%s", aid);
    /* construct the filepath for the end file */
    sprintf(filepath, "%s/END_%s.txt", dirname, aid);
    /* create the end file using the provided utility function create_file */
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

int create_bid_value(const char *aid, int bid_value, char *fdata) {
    char filepath[FILEPATH];
    char dirname[DIRNAME];
    
    /* construct the directory path for the auction */
    sprintf(dirname, "AUCTIONS/%s/BIDS", aid);
    /* construct the filepath for the bid file */
    sprintf(filepath, "%s/%06d.txt", dirname, bid_value);
    /* create the bid file using the provided utility function create_file */
    if (create_file(filepath, dirname, fdata) == -1)
        return 0;
    return 1;
}

//////////////////////////////////// MULTIPLE FILE CREATION OR CONDITIONAL ////////////////////////////////////////////

int create_open_files(const char *aid, const char *uid, char *fdata) {
    time_t fulltime;
    char time_str[DATE_TIME+1];

    time(&fulltime);                      // get current time in seconds starting at 1970
    convert_to_date(fulltime, time_str);  // convert the current time to a formatted date string

    /* append time and timestamp to the existing fdata */
    sprintf(fdata + strlen(fdata), "%s %ld\n", time_str, fulltime);

    /* create hosted and start files using utility functions */
    if (!create_hosted(uid, aid) || !create_start(aid, fdata))
        return 0;
    return 1;
}

int create_end_if_expired(const char *aid, int timeactive, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time;
    time_t limit_time = (time_t) (starttime + (long) timeactive);
    char time_str[DATE_TIME+1];
    
    time(&actual_time); // get actual time in seconds

    /* check if the auction has expired */
    if (actual_time >= limit_time) {
        /* convert the expiration time to a formatted date string */
        convert_to_date(limit_time, time_str);
        /* format data for the end file */
        sprintf(fdata, "%s %d\n", time_str, timeactive);
        /* create the end file using the utility function */
        if (!create_end(aid, fdata))
            return 0;
    }
    return 1;
}

int create_end_close(const char *aid, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time;
    char time_str[DATE_TIME+1];
    
    time(&actual_time);                                         // get actual time in seconds
    convert_to_date(actual_time, time_str);                     // convert the current time to a formatted date string
    int timeactive = (int) (actual_time - (time_t) starttime);  // calculate the time the auction has been active

    /* format data for the end file */
    sprintf(fdata, "%s %d\n", time_str, timeactive);

    /* create the end file using the utility function */
    if (!create_end(aid, fdata))
        return 0;
    return 1;
}

int create_bid_files(const char *uid, const char *aid, const char *value, long starttime) {
    char fdata[BUFSIZ];
    time_t actual_time, bid_sec_time;
    char time_str[DATE_TIME+1];
    int bid_value = atoi(value);
    
    time(&actual_time);                                         // get actual time in seconds
    bid_sec_time = (time_t) (actual_time - (time_t) starttime); // calculate the time since the auction started
    convert_to_date(actual_time, time_str);                     // convert the current time to a formatted date string

    /* format data for the bid file */
    sprintf(fdata, "%s %s %s %ld\n", uid, value, time_str, bid_sec_time);

    /* create bid value and bidded files using utility functions */
    if (!create_bid_value(aid, bid_value, fdata) || !create_bidded(uid, aid))
        return 0;
    return 1;
}
