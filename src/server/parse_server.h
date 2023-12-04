#ifndef __PARSE_SERVER_H__
#define __PARSE_SERVER_H__

int confirm_login(const char *buffer, char *uid, char *pass, char *msg);

int confirm_open(const char *uid, const char *pass, const char *start_time, const char *name,
                const char *time_active, const char *fname, const char *fsize, char *msg);

int confirm_list_my(const char *buffer, char *uid, char *msg);

#endif // __PARSE_SERVER_H__