#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

int read_password_file(const char *fname, char *existing_pass);

void verify_all_end();
int verify_auction_end(const char *aid);

int read_start_file(const char *path, char *uid, char *name, char *fname, int *start_value,
                    int *timeactive, char *datetime, long *starttime);

int read_start(const char *fname_start, char *msg);

/// --- UDP

void process_login(const char *uid, const char *pass, char *msg);

void process_logout(const char *uid, const char *pass, char *msg);

void process_unregister(const char *uid, const char *pass, char *msg);

void get_auctions(const char *dirname, const char *cmd, char *msg);

void process_ma(const char *uid, char *msg);

void process_mb(const char *uid, char *msg);

void process_list(char *msg);

void process_sr(const char *aid, char *msg);


/// --- TCP

int process_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *aid, char *buffer);

void process_close(const char *uid, const char *pass, const char *aid, char *buffer);

int process_sa(const char *aid, char *fname, long *fsize, char *msg);

void get_highest_bid(const char *dirname, const char *start_file, char *bid_value);

void process_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer);


#endif // __PROCESS_SERVER_H__
