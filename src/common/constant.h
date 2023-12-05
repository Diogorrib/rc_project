#ifndef __CONSTANT_H__
#define __CONSTANT_H__

#define DEFAULT_PORT "58064"    // 58000 + 64 (group number)
#define DEFAULT_IP "localhost"  // tejo.tecnico.ulisboa.pt or localhost

#define NI_MAXHOST 1025
#define NI_MAXSERV 32

#define UDP_TIMEOUT 3
#define TCP_TIMEOUT 5

#define BUFFER_512 512
#define FIRST_WORD 16
#define MAX_4_SOME_INTS 6   // 6 is the max digits of some numbers
#define DATE 10
#define HOUR 8

#define UID 6
#define PASSWORD 8
#define AID 3
#define CMD_N_SPACE 4   // 3 chars plus a space
#define STATUS 4        // 3 chars (worst case) plus other char
#define NAME 10         // description is max 10 chars
#define FNAME 24        // file names are limited to a total of 24 characters
#define FSIZE 8         // max file size is 10MB (10^7 have 8 digits max)
#define DATE_TIME 19    // format YYYY-MM-DD HH:MM:SS
#define MAX_AUCTION 999 // max possible auctions
#define MAX_BIDS 50     // max bids for show record
#define BID (UID+MAX_4_SOME_INTS+DATE_TIME+MAX_4_SOME_INTS+6)   // including the next space (B bidder_UID bid_value bid_date-time bid_sec_time)

#define LOGIN_SND (CMD_N_SPACE+UID+PASSWORD+3) // size of message sent to AS including the '\0' (ex.: LIN UID password)
#define OPEN_SND (CMD_N_SPACE+UID+PASSWORD+NAME+2*MAX_4_SOME_INTS+FNAME+FSIZE+8) // size of a part of the message sent to AS including the '\0' (OPA UID password name start_value timeactive Fname Fsize ...)
#define CLOSE_SND (CMD_N_SPACE+UID+PASSWORD+AID+4) // size of message sent to AS including the '\0' (CLS UID password AID)
#define MY_SND (CMD_N_SPACE+UID+2)  // size of message sent to AS including the '\0' (ex.: LMA UID)
#define SHOW_SND (CMD_N_SPACE+AID+2)  // size of message sent to AS including the '\0' (ex.: SRC AID)
#define BID_SND (CMD_N_SPACE+UID+PASSWORD+AID+MAX_4_SOME_INTS+5) // size of the message sent to AS including the '\0' (BID UID password AID value)

#define LOGIN_RCV (CMD_N_SPACE+STATUS+1)    // size of message received including the '\0' (ex.: RLI status)
#define OPEN_RCV (CMD_N_SPACE+STATUS+AID+2) // size of message received including the '\0' (ROA status AID)
#define CLS_RCV (CMD_N_SPACE+STATUS+1) // size of message received including the '\0' (RCL status)
#define LST_RCV (CMD_N_SPACE+STATUS+ MAX_AUCTION * (AID+3) + 2) // size of message received including the '\0' (ex.: RLS status[ AID state]*)
#define SA_RCV (CMD_N_SPACE+STATUS) // size of a part of the message received including the '\0' (RSA status...)
#define BID_RCV (CMD_N_SPACE+STATUS+1) // size of message received including the '\0' (RBD status)
/** size of message received including the '\0'
 * RRC status [host_UID auction_name asset_fname start_value start_date-time timeactive]
 * [ B bidder_UID bid_value bid_date-time bid_sec_time]*
 * [ E end_date-time end_sec_time]*/
#define SR_RCV (CMD_N_SPACE+STATUS+UID+NAME+FNAME+MAX_4_SOME_INTS+DATE_TIME+MAX_4_SOME_INTS+5+ \
                BID*MAX_BIDS+ \
                DATE_TIME+MAX_4_SOME_INTS+6)

#define LST_PRINT (CMD_N_SPACE+11)  // 3 chars plus a space + strlen("not active\n") (worst case)
#define SR_PRINT (NAME+FNAME+UID+MAX_4_SOME_INTS+DATE_TIME+MAX_4_SOME_INTS+MAX_BIDS*(MAX_4_SOME_INTS+MAX_4_SOME_INTS+DATE_TIME+MAX_4_SOME_INTS+59)+71+32+1) // worst case for show record print including '\0'

#endif // __CONSTANT_H__
