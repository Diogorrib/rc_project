#ifndef __FILE_CREATION_H__
#define __FILE_CREATION_H__


int create_login(const char *uid);
int create_password(const char *uid, const char *pass);
int create_hosted(const char *uid, const char *aid);
int create_bidded(const char *uid, const char *aid);
int create_start(const char *aid, const char *fdata);
int create_asset(int fd, const char *aid, const char *fname, const char *fsize);
int create_end(const char *aid, int timeactive, long starttime);
int create_bid_value(const char *uid, const char *aid, const char *value, long starttime);
int create_open_files(const char *aid, const char *uid, char *fdata);

#endif // __FILE_CREATION_H__
