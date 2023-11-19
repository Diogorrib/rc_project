#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#define DEFAULT_PORT "58011"    // 58000 + GN
#define DEFAULT_IP "tejo.tecnico.ulisboa.pt"

#define UID 6
#define PASSWORD 8
#define AID 3
#define CMD_N_SPACE 4   // 3 chars plus a space
#define STATUS 4        // 3 chars (worst case) plus other char
#define NAME 10         // description is max 10 chars
#define MAX_AUCTION 999 // max possible auctions

#define LOGIN_SND (CMD_N_SPACE+UID+PASSWORD+3) // size of message sent to AS including the '\0'
#define OPEN_SND (CMD_N_SPACE+UID+PASSWORD+NAME+6+6+19+8) // size of a part of the message sent to AS including the '\0' (6 is the max digits of some numbers and 19 is the max digits of a long int)

#define LOGIN_RCV (CMD_N_SPACE+STATUS+1)    // size of message received including the '\0'
#define LST_RCV (7 + MAX_AUCTION * (AID+3)) // 7 initial chars plus MAX * (AID + 1 char(state) and 2 spaces)
#define OPEN_RCV (CMD_N_SPACE+STATUS+AID+2) // size of message received including the '\0'

#define LST_PRINT (CMD_N_SPACE+11)  // 3 chars plus a space + strlen("not active\n") (worst case)

#endif // __CONSTANT_H__
