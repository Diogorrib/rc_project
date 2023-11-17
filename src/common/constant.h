#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#define DEFAULT_PORT "58011"    // 58000 + GN
#define DEFAULT_IP "tejo.tecnico.ulisboa.pt"

#define UID 6
#define PASSWORD 8
#define LOGIN_MSG 9
#define CMD_N_SPACE 4
#define LST_MSG (7 + 999 * 6) // 7 initial bits plus the [AID state ] times the 999 (max possible auctions)
 

#endif // __CONSTANT_H__
