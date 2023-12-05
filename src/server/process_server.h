#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

int read_password_file(const char *fname, char *existing_pass);

void verify_auction_end();

/// --- UDP

void process_login(const char *uid, const char *pass, char *msg);

void process_logout(const char *uid, const char *pass, char *msg);

void process_unregister(const char *uid, const char *pass, char *msg);

void get_auctions(const char *dirname, const char *cmd, char *msg);

void process_ma(const char *uid, char *msg);

void process_mb(const char *uid, char *msg);

void process_list(char *msg);


/// --- TCP

int process_open(const char *uid, const char *pass, const char *name, const char *start_value,
                const char *timeactive, const char *fname, const char *aid, char *buffer);

void get_highest_bid(const char *dirname, const char *start_file, char *bid_value);

void process_bid(const char *uid, const char *pass, const char *aid, const char *bid_value, char *buffer);


#endif // __PROCESS_SERVER_H__
