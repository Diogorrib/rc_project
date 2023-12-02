#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

int read_password_file(const char *fname, char *existing_pass);

void process_login(const char *uid, const char *pass, char *msg);

void process_logout(const char *uid, const char *pass, char *msg);

void process_unregister(const char *uid, const char *pass, char *msg);

#endif // __PROCESS_SERVER_H__
