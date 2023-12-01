#ifndef __FILE_CREATION_H__
#define __FILE_CREATION_H__

int create_login(const char *uid);
int create_password(const char *uid, const char *pass);
int create_hosted(const char *uid, const char *aid);
int create_bidded(const char *uid, const char *aid);
int create_start(const char *aid, const char *fdata);
int create_asset(const char *aid, const char *fname, const char *fdata);
int create_end(const char *aid, const char *fdata);
int create_bid_value(const char *aid, const char *value, const char *fdata);

#endif // __FILE_CREATION_H__
