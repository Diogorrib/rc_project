#include "server.h"
#include "file_creation.h"
#include "process_server.h"
#include "parse_server.h"
#include <sys/wait.h>

int verbose_mode = 0; // if zero verbose mode is off else is on
char *as_port = DEFAULT_PORT;
int aid = 1;

///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////

void login(char *buffer, char *msg) {
    char uid[UID+1];
    char pass[PASSWORD+1];

    if(confirm_login(buffer, uid, pass, msg) == -1)
        return;
    
    process_login(uid, pass, msg);
}

void logout(char *buffer, char *msg) {
    char uid[UID+1];
    char pass[PASSWORD+1];

    if(confirm_login(buffer, uid, pass, msg) == -1) 
        return;
    
    process_logout(uid, pass, msg); 
}

void unregister(char *buffer, char *msg) {
    char uid[UID+1];
    char pass[PASSWORD+1];

    if(confirm_login(buffer, uid, pass, msg) == -1) 
        return;
    
    process_unregister(uid, pass, msg);
}

void myauctions(char *buffer, char *msg) { 
    char uid[UID+1];

    if(confirm_list_my(buffer,uid,msg) == -1)
        return;

    verify_all_end();

    process_ma(uid,msg);
}

void mybids(char *buffer, char *msg) { 
    char uid[UID+1];

    if(confirm_list_my(buffer,uid,msg) == -1)
        return;

    verify_all_end();

    process_mb(uid,msg);
}

void list(char *msg) {

    verify_all_end();
    
    process_list(msg);
}

void show_record(char *buffer, char *msg) { 
    char aid_record[AID+1];
    
    if(confirm_sr(buffer,aid_record,msg) == -1)
        return;

    verify_auction_end(aid_record);
    
    process_sr(aid_record, msg);
}

void parse_udp_buffer(char *buffer, char *msg) {
    char cmd[CMD_N_SPACE+1];
    memset(msg, '\0', SR_RCV);
    if(buffer[LOGIN_SND-1] != '\0') {
        sprintf(msg, "ERR\n");
        return;
    }
    // get the action to do
    memset(cmd, '\0', CMD_N_SPACE+1);
    memcpy(cmd, buffer, CMD_N_SPACE);

    /* Compare cmd with the list of possible udp actions */
    if (!strcmp("LIN ", cmd))
        login(buffer, msg);
    else if (!strcmp("LOU ", cmd))
        logout(buffer, msg);
    else if (!strcmp("UNR ", cmd))
        unregister(buffer, msg);
    else if (!strcmp("LMA ", cmd))
        myauctions(buffer, msg);
    else if (!strcmp("LMB ", cmd))
        mybids(buffer, msg);
    else if (!strcmp("LST\n", cmd))
        list(msg);
    else if (!strcmp("SRC ", cmd))
        show_record(buffer, msg);
    else
        sprintf(msg, "ERR\n");
}

void udp() {
    fd_set inputs, testfds;
    struct timeval timeout;
    int fd,out_fds,errcode;
    ssize_t nread,nwritten;
    char buffer[LOGIN_SND], msg_sent[SR_RCV];
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char host[NI_MAXHOST], service[NI_MAXSERV];

    // UDP SERVER SECTION
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    errcode=getaddrinfo(NULL,as_port,&hints,&res);
    if(errcode != 0) {
        printf("ERR: UDP: getaddrinfo\n"); return;
    }

    fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(fd == -1) {
        printf("ERR: UDP: socket\n"); return;
    }

    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1) {
        printf("ERR: UDP: bind\n"); close(fd); return;
    }
    if(res!=NULL) freeaddrinfo(res);

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(fd,&inputs); // Set UDP channel on

    printf("UDP started\n"); ///////////////////////////////////////////////////////////////////////////////////////////////

    while(1) {
        testfds=inputs; // Reload mask
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=BIG_TIMEOUT;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        switch(out_fds) {
            case 0:
                break;
            case -1:
                printf("ERR: UDP: select\n");
                freeaddrinfo(res); close(fd); return;
            default:
                if(FD_ISSET(fd,&testfds)) {
                    addrlen=sizeof(addr);
                    
                    if (set_recv_timeout(fd, SERVER_UDP_TIMEOUT) == -1) {
                        printf("ERR: UDP: recv_timeout\n");
                        freeaddrinfo(res); close(fd); return;
                    }
                    /* Receive message from user */
                    memset(buffer, '\0', LOGIN_SND);
                    nread=recvfrom(fd,buffer,LOGIN_SND,0,(struct sockaddr*)&addr,&addrlen);
                    if(nread == -1) {
                        printf("ERR: UDP: recvfrom\n");
                        freeaddrinfo(res); close(fd); return;
                    }

                    errcode=getnameinfo((struct sockaddr*)&addr,addrlen,host,sizeof host,service,sizeof service,0);
                    if(errcode==0)
                        request_received(buffer,host,service,verbose_mode);
                    else {
                        printf("ERR: UDP: getnameinfo\n");
                        freeaddrinfo(res); close(fd); return;
                    }
                    
                    parse_udp_buffer(buffer, msg_sent);
                    
                    if (set_send_timeout(fd, SERVER_UDP_TIMEOUT) == -1) {
                        printf("ERR: UDP: send_timeout\n");
                        freeaddrinfo(res); close(fd); return;
                    }
                    /* Send message to user */
                    nwritten=sendto(fd,msg_sent,strlen(msg_sent),0,(struct sockaddr*)&addr,addrlen);
                    if(nwritten == -1) {
                        printf("ERR: UDP: sendto\n");
                        freeaddrinfo(res); close(fd); return;
                    }
                }
                
        }
    }
    freeaddrinfo(res);
    close(fd);
}

///////////////////////////////////////////////// TCP /////////////////////////////////////////////////////////////////

int read_from_tcp(int fd, char *buffer, int to_read) {
    ssize_t nleft,nread;
    char aux[1], *ptr;

    nleft=to_read; ptr=buffer;
    while (nleft>0) {
        if (set_recv_timeout(fd, SERVER_TCP_TIMEOUT) == -1) {
            printf("ERR: TCP: read timeout\n");
            return -1;
        }
        nread=read(fd,aux,1);
        if(nread <= 0) {    // closed by user or timeout event
            printf("ERR: TCP: did not receive all message\n");
            return -1;
        }
        if (nleft == 1 && aux[0] != ' ' && aux[0] != '\n') {
            printf("ERR: TCP: not a space\n"); // last char before the limit need to be a space
            return -1;
        }
        if(aux[0] == ' ' || aux[0] == '\n') {
            if (nleft == to_read) {
                printf("ERR: TCP: empty buffer\n"); // first char is a space
                return -1;
            }
            break; // read a space
        }
        memcpy(ptr, aux, 1);
        nleft-=nread; ptr+=nread;
    }
    return 0;
}

int write_to_tcp(int fd, char *buffer) {
    ssize_t nleft,nwritten;
    char *ptr;
    struct sigaction act;

    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;
    if (sigaction(SIGPIPE,&act,NULL) == -1) {//error
        printf("ERR: TCP: sigaction\n");
        return -1;
    }
    nleft=(ssize_t)strlen(buffer); ptr=buffer;
    while (nleft>0) {
        if (set_send_timeout(fd, SERVER_TCP_TIMEOUT) == -1) {
            printf("ERR: TCP: write timeout\n");
            return -1;
        }
        nwritten=write(fd,ptr,(size_t)nleft);
        if(nwritten <= 0) {    // closed by user or timeout event
            printf("ERR: TCP: did not receive all message\n");
            return -1;
        }
        nleft-=nwritten; ptr+=nwritten;
    }
    return 0;
}

void open_auction(int fd, char *buffer) {
    char uid[UID+1], pass[PASSWORD+1], name[NAME+1];
    char start_value[MAX_4_SOME_INTS+1], timeactive[MAX_4_SOME_INTS+1];
    char fname[FNAME+1], fsize[FSIZE+1];
    char aid_str[AID+1];

    memset(uid, '\0', UID+1);
    if (read_from_tcp(fd, uid, UID+1) == -1)
        return;
    memset(pass, '\0', PASSWORD+1);
    if (read_from_tcp(fd, pass, PASSWORD+1) == -1)
        return;
    memset(name, '\0', NAME+1);
    if (read_from_tcp(fd, name, NAME+1) == -1)
        return;
    memset(start_value, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp(fd, start_value, MAX_4_SOME_INTS+1) == -1)
        return;
    memset(timeactive, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp(fd, timeactive, MAX_4_SOME_INTS+1) == -1)
        return;
    memset(fname, '\0', FNAME+1);
    if (read_from_tcp(fd, fname, FNAME+1) == -1)
        return;
    memset(fsize, '\0', FSIZE+1);
    if (read_from_tcp(fd, fsize, FSIZE+1) == -1)
        return;

    memset(buffer, '\0', OPEN_RCV); // reset buffer
    if(confirm_open(uid, pass, name, start_value, timeactive, fname, fsize, buffer) == -1) 
        return;
    
    sprintf(aid_str, "%03d", aid);
    if(!create_asset(fd, aid_str, fname, fsize)) {
        sprintf(buffer, "ERR\n");
        return;
    }

    if (process_open(uid, pass, name, start_value, timeactive, fname, aid_str, buffer) == 0)
        aid++;
}

void close_auction(int fd, char *buffer) { 
    char uid[UID+1], pass[PASSWORD+1], aid_auction[AID+1];

    memset(uid, '\0', UID+1);
    if (read_from_tcp(fd, uid, UID+1) == -1)
        return;
    memset(pass, '\0', PASSWORD+1);
    if (read_from_tcp(fd, pass, PASSWORD+1) == -1)
        return;
    memset(aid_auction, '\0', AID+1);
    if (read_from_tcp(fd, aid_auction, AID+1) == -1)
        return;

    memset(buffer, '\0', OPEN_RCV); // reset buffer
    if(confirm_close(uid, pass, aid_auction, buffer) == -1) 
        return;

    verify_auction_end(aid_auction);

    process_close(uid, pass, aid_auction, buffer);
}

/* if return is 0 message is NOK else send the asset (inside ths function) */
int show_asset(int fd, char *buffer) {
    char aid_auction[AID+1], fname[FNAME+1], filepath[64];
    long fsize;
    
    memset(aid_auction, '\0', AID+1);
    if (read_from_tcp(fd, aid_auction, AID+1) == -1)
        return 0;

    memset(buffer, '\0', OPEN_RCV); // reset buffer
    if(confirm_sa(aid_auction, buffer) == -1) 
        return 0;

    if (process_sa(aid_auction, fname, &fsize, buffer) == -1)
        return 0;

    /* Send the 'RSA OK fname fsize ' */
    sprintf(buffer, "RSA OK ");
    write_to_tcp(fd, buffer);

    sprintf(filepath,"AUCTIONS/%s/%s", aid_auction, fname);
    write_to_tcp(fd, fname);

    memset(buffer, '\0', OPEN_RCV);
    sprintf(buffer, " %ld ", fsize);
    write_to_tcp(fd, buffer);

    /* Send the asset file and the \n */
    send_file(fd, filepath, fsize);
    write_to_tcp(fd, "\n");
    return 1;
}

void bid(int fd, char *buffer) {
    char uid[UID+1], pass[PASSWORD+1], aid_bid[AID+1];
    char bid_value[MAX_4_SOME_INTS+1];

    memset(uid, '\0', UID+1);
    if (read_from_tcp(fd, uid, UID+1) == -1)
        return;
    memset(pass, '\0', PASSWORD+1);
    if (read_from_tcp(fd, pass, PASSWORD+1) == -1)
        return;
    memset(aid_bid, '\0', AID+1);
    if (read_from_tcp(fd, aid_bid, AID+1) == -1)
        return;
    memset(bid_value, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp(fd, bid_value, MAX_4_SOME_INTS+1) == -1)
        return;

    memset(buffer, '\0', OPEN_RCV); // reset buffer
    if(confirm_bid(uid, pass, aid_bid, bid_value, buffer) == -1) 
        return;

    verify_auction_end(aid_bid);
    
    process_bid(uid, pass, aid_bid, bid_value, buffer);
}

void parse_tcp_buffer(int fd, char *buffer, struct sockaddr_in addr, socklen_t addrlen) {
    char cmd[CMD_N_SPACE];
    char host[NI_MAXHOST], service[NI_MAXSERV];
    int errcode;

    // get the action to do
    memset(cmd, '\0', CMD_N_SPACE);
    if (read_from_tcp(fd, cmd, CMD_N_SPACE) == -1)
        return;

    // get info about action to do and about the peer
    errcode=getnameinfo((struct sockaddr*)&addr,addrlen,host,sizeof host,service,sizeof service,0);
    if(errcode==0)
        request_received(cmd,host,service,verbose_mode);
    else {
        printf("ERR: TCP: getnameinfo\n");
        return;
    }

    /* Compare cmd with the list of possible udp actions */
    memset(buffer, '\0', OPEN_RCV);
    if (!strcmp("OPA", cmd)) 
        open_auction(fd, buffer);
    else if (!strcmp("CLS", cmd))
        close_auction(fd, buffer);
    else if (!strcmp("SAS", cmd)) {
        if (show_asset(fd, buffer))
            return;
    }
    else if (!strcmp("BID", cmd))
        bid(fd, buffer);
    else
        sprintf(buffer, "ERR\n");
    
    write_to_tcp(fd, buffer);
}

void tcp() {
    fd_set inputs, testfds;
    struct timeval timeout;
    int fd,errcode,out_fds;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char msg_sent[OPEN_RCV];

// TCP SERVER SECTION
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    errcode=getaddrinfo(NULL,as_port,&hints,&res);
    if(errcode != 0) {
        printf("ERR: TCP: getaddrinfo\n"); return;
    }

    fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(fd == -1) {
        printf("ERR: TCP: socket\n"); return;
    }

    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1) {
        printf("ERR: TCP: bind\n"); close(fd); return;
    }
    if(res!=NULL) freeaddrinfo(res);

    if(listen(fd,5) == -1) {
        printf("ERR: TCP: listen\n"); close(fd); return;
    }

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(fd,&inputs); // Set TCP channel on

    printf("TCP started\n"); ///////////////////////////////////////////////////////////////////////////////////////////////

    while(1) {
        testfds=inputs; // Reload mask
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=BIG_TIMEOUT;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        switch(out_fds) {
            case 0:
                break;
            case -1:
                printf("ERR: UDP: select\n");
                freeaddrinfo(res); close(fd); return;
            default:
                if(FD_ISSET(fd,&testfds)) {
                    int new_fd;
                    addrlen = sizeof(addr);
                    if((new_fd=accept(fd, (struct sockaddr*)&addr, &addrlen))==-1) {
                        printf("ERR: TCP: accept\n"); freeaddrinfo(res); close(fd); return;
                    }

                    parse_tcp_buffer(new_fd, msg_sent, addr, addrlen);

                    close(new_fd);
                }
        }
    }
    freeaddrinfo(res);
    close(fd);
}

///////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////////

void filter_input(int argc, char **argv) {
    if (argc == 1) // only one argument no need for updates
        return;
    for (int i = 1; i < argc; i++) {
        /* update the port where AS app is running */
        if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];

        else if (!strcmp(argv[i], "-v")) {
            verbose_mode = 1;
        }
    }
}

int main(int argc, char **argv) {

    filter_input(argc, argv);

    pid_t childPid;

    /* Create main directories */
    if(!verify_directory("USERS"))
        if (mkdir("USERS", 0700) == -1)
            return -1;
    if(!verify_directory("AUCTIONS"))
        if (mkdir("AUCTIONS", 0700) == -1)
            return -1;

    for(int i = 1; i < 999; i++) {
        char dirname[20];
        sprintf(dirname, "AUCTIONS/%03d", i);
        if(!verify_directory(dirname))
            break;
        aid++;
    }
    printf("next %03d\n", aid);

    switch (childPid = fork()) {
    case -1:
        printf("ERR: creating child process");
        return -1;

    case 0: // udp process
        udp();
        break;

    default: // tcp process
        tcp();
        pid_t child_pid = waitpid(-1, NULL, 0); // wait for child process to end
        (void) child_pid;
    }
    return 0;
}
