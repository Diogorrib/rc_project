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
#define FNAME 24        // file names are limited to a total of 24 characters
#define DATE_TIME 19    // format YYYY-MM-DD HH:MM:SS
#define MAX_AUCTION 999 // max possible auctions
#define MAX_BIDS 50     // max possible auctions
#define BID (2+UID+19+DATE_TIME+6+4)   // including the next space (6 is the max digits of some numbers)

#define LOGIN_SND (CMD_N_SPACE+UID+PASSWORD+3) // size of message sent to AS including the '\0'
#define OPEN_SND (CMD_N_SPACE+UID+PASSWORD+NAME+6+6+FNAME+19+8) // size of a part of the message sent to AS including the '\0' (6 is the max digits of some numbers and 19 is the max digits of a long int)
#define CLOSE_SND (CMD_N_SPACE+UID+PASSWORD+AID+4) // size of message sent to AS including the '\0'
#define MY_SND (CMD_N_SPACE+UID+2)  // size of message sent to AS including the '\0'
#define SHOW_SND (CMD_N_SPACE+AID+2)  // size of message sent to AS including the '\0'
#define BID_SND (CMD_N_SPACE+UID+PASSWORD+AID+19+5) // size of the message sent to AS including the '\0' 

#define LOGIN_RCV (CMD_N_SPACE+STATUS+1)    // size of message received including the '\0'
#define OPEN_RCV (CMD_N_SPACE+STATUS+AID+2) // size of message received including the '\0'
#define CLS_RCV (CMD_N_SPACE+STATUS+1) // size of message received including the '\0'
#define LST_RCV (7 + MAX_AUCTION * (AID+3)) // 7 initial chars plus MAX * (AID + 1 char(state) and 2 spaces)
#define SA_RCV (CMD_N_SPACE+STATUS) // size of message received including the '\0'
#define BID_RCV (CMD_N_SPACE+STATUS+1) // size of message received including the '\0'
#define SR_RCV (CMD_N_SPACE+STATUS+UID+NAME+FNAME+6+DATE_TIME+6+7+BID*MAX_BIDS+2+DATE_TIME+6+2) // including the space before the first B (6 is the max digits of some numbers)

#define LST_PRINT (CMD_N_SPACE+11)  // 3 chars plus a space + strlen("not active\n") (worst case)
#define SR_PRINT (NAME+FNAME+UID+6+DATE_TIME+6+MAX_BIDS*(6+19+DATE_TIME+6+59)+71+32+1) 

#endif // __CONSTANT_H__
