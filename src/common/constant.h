#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#define DEFAULT_PORT "58011"    // 58000 + GN
#define DEFAULT_IP "tejo.tecnico.ulisboa.pt"

#define UID 6
#define PASSWORD 8
#define LOGIN_MSG 9     // size of login (and logout, ...) message including the '\0'
#define CMD_N_SPACE 4   // 3 chars plus a space
#define STATUS 4
#define MAX_AUCTION 999 // max possible auctions
#define LST_MSG (7 + MAX_AUCTION * 6)   // 7 initial chars plus the [AID state ] * MAX_AUCTION
#define LST_PRINT 15    // 3 digits + strlen(" not active\n") (worst case)

#endif // __CONSTANT_H__
