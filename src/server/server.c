#include "server.h"
#include "file_creation.h"
#include "process_server.h"
#include "parse_server.h"
#include <sys/wait.h>

int verbose_mode = 0; // if zero verbose mode is off else is on
char *as_port = DEFAULT_PORT;
int aid = 1;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    /* The message receive from the user is equal to the login comand.   */
    /* The only diference is the command of the 3 initial letters, so we */
    /* can use the function confirm_login() here.                        */
    if(confirm_login(buffer, uid, pass, msg) == -1) 
        return;
    
    process_logout(uid, pass, msg); 
}

void unregister(char *buffer, char *msg) {
    char uid[UID+1];
    char pass[PASSWORD+1];

    /* The message receive from the user is equal to the login comand.   */
    /* The only diference is the command of the 3 initial letters, so we */
    /* can use the function confirm_login() here.                        */
    if(confirm_login(buffer, uid, pass, msg) == -1) 
        return;
    
    process_unregister(uid, pass, msg);
}

void myauctions(char *buffer, char *msg) { 
    char uid[UID+1];

    if(confirm_list_my(buffer,uid,msg) == -1)
        return;

    verify_auction_end();

    process_ma(uid,msg);
}

void mybids(char *buffer, char *msg) { 
    char uid[UID+1];

    if(confirm_list_my(buffer,uid,msg) == -1)
        return;

    verify_auction_end();

    process_mb(uid,msg);
}

void list(char *msg) {

    verify_auction_end();
    
    process_list(msg);
}

void show_record(char *buffer, char *msg) { printf("TODO: show_record"); (void)buffer; (void)msg; } // TEMOS DE VERIFICAR SE AS AÇÔES ACABARAM

void parse_udp_buffer(char *buffer, char *msg) {
    char cmd[CMD_N_SPACE+1];
    memset(msg, '\0', SR_RCV);
    if(buffer[LOGIN_SND-1] != '\0') {
        sprintf(msg, "ERR\n");
        return;
    }
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
    struct timeval timeout,send_timeout,recv_timeout;
    int fd,out_fds,errcode;
    ssize_t nread,nwritten,n;
    char buffer[LOGIN_SND], msg_sent[SR_RCV];
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char host[NI_MAXHOST], service[NI_MAXSERV];

    (void)n;
    (void)send_timeout;

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
        timeout.tv_sec=UDP_TIMEOUT;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        switch(out_fds) {
            case 0:
                //printf("\n ---------------UDP timeout event-----------------\n");
                break;
            case -1:
                printf("ERR: UDP: select\n");
                freeaddrinfo(res); close(fd); return;
            default:
                if(FD_ISSET(fd,&testfds)) {
                    addrlen=sizeof(addr);
                    /* Set receive timeout */
                    recv_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
                    recv_timeout.tv_usec = 0;
                    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0) {
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
                    
                    /* Set send timeout */ 
                    send_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
                    send_timeout.tv_usec = 0;
                    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout)) < 0) {
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// TCP /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int read_from_tcp(int fd, char *buffer, long to_read) {
    struct timeval recv_timeout;
    ssize_t nleft,nread;
    char *ptr;

    nleft=to_read; ptr=buffer;
    while (nleft>0) {
        /* Set receive timeout */
        recv_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
        recv_timeout.tv_usec = 0;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0) {
            printf("ERR: TCP: read timeout\n");
            return -1;
        }
        nread=read(fd,ptr,(size_t)nleft);
        if(nread <= 0) {    // closed by user or timeout event
            printf("ERR: TCP: did not receive all message\n");
            return -1;
        }
        nleft-=nread; ptr+=nread;
    }
    return 0;
}

int read_from_tcp_spaces(int fd, char *buffer, int to_read) {
    struct timeval recv_timeout;
    ssize_t nleft,nread;
    char aux[1], *ptr;

    nleft=to_read; ptr=buffer;
    while (nleft>0) {
        /* Set receive timeout */
        recv_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
        recv_timeout.tv_usec = 0;
        if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0) {
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
    struct timeval send_timeout;
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
        /* Set receive timeout */ 
        send_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
        send_timeout.tv_usec = 0;
        if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout)) < 0) {
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
    if (read_from_tcp_spaces(fd, uid, UID+1) == -1)
        return;
    memset(pass, '\0', PASSWORD+1);
    if (read_from_tcp_spaces(fd, pass, PASSWORD+1) == -1)
        return;
    memset(name, '\0', NAME+1);
    if (read_from_tcp_spaces(fd, name, NAME+1) == -1)
        return;
    memset(start_value, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp_spaces(fd, start_value, MAX_4_SOME_INTS+1) == -1)
        return;
    memset(timeactive, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp_spaces(fd, timeactive, MAX_4_SOME_INTS+1) == -1)
        return;
    memset(fname, '\0', FNAME+1);
    if (read_from_tcp_spaces(fd, fname, FNAME+1) == -1)
        return;
    memset(fsize, '\0', FSIZE+1);
    if (read_from_tcp_spaces(fd, fsize, FSIZE+1) == -1)
        return;

    memset(buffer, '\0', OPEN_RCV);
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

void close_auction(int fd, char *buffer) { (void)fd; (void)buffer; printf("TODO: close_auction\n"); } // SE CALHAR TEMOS DE VERIFICAR SE AS AÇÔES ACABARAM
void show_asset(int fd, char *buffer) { (void)fd; (void)buffer; printf("TODO: show_asset\n"); }

void bid(int fd, char *buffer) {
    char uid[UID+1], pass[PASSWORD+1], aid_bid[AID+1];
    char bid_value[MAX_4_SOME_INTS+1];

    memset(uid, '\0', UID+1);
    if (read_from_tcp_spaces(fd, uid, UID+1) == -1)
        return;
    memset(pass, '\0', PASSWORD+1);
    if (read_from_tcp_spaces(fd, pass, PASSWORD+1) == -1)
        return;
    memset(aid_bid, '\0', AID+1);
    if (read_from_tcp_spaces(fd, aid_bid, AID+1) == -1)
        return;
    memset(bid_value, '\0', MAX_4_SOME_INTS+1);
    if (read_from_tcp_spaces(fd, bid_value, MAX_4_SOME_INTS+1) == -1)
        return;

    memset(buffer, '\0', BID_RCV);
    if(confirm_bid(uid, pass, aid_bid, bid_value, buffer) == -1) 
        return;

    verify_auction_end();
    
    process_bid(uid, pass, aid_bid, bid_value, buffer);
}

void parse_tcp_buffer(int fd, char *buffer, struct sockaddr_in addr, socklen_t addrlen) {
    char cmd[CMD_N_SPACE+1];
    char host[NI_MAXHOST], service[NI_MAXSERV];
    int errcode;

    memset(cmd, '\0', CMD_N_SPACE+1);
    
    if (read_from_tcp(fd, cmd, CMD_N_SPACE) == -1)
        return;

    memset(buffer, '\0', OPEN_RCV);
    errcode=getnameinfo((struct sockaddr*)&addr,addrlen,host,sizeof host,service,sizeof service,0);
    if(errcode==0)
        request_received(cmd,host,service,verbose_mode);
    else {
        printf("ERR: TCP: getnameinfo\n");
        return;
    }

    /* Compare cmd with the list of possible udp actions */
    if (!strcmp("OPA ", cmd))
        open_auction(fd, buffer);
    else if (!strcmp("CLS ", cmd))
        close_auction(fd, buffer);
    else if (!strcmp("SAS ", cmd))
        show_asset(fd, buffer);
    else if (!strcmp("BID ", cmd))
        bid(fd, buffer);
    else
        printf("ERR: TODO %s\n", cmd);
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
        timeout.tv_sec=TCP_TIMEOUT;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        switch(out_fds) {
            case 0:
                //printf("\n ---------------TCP timeout event-----------------\n");
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

                    write_to_tcp(new_fd, msg_sent);
                    
                    close(new_fd);
                }
        }
    }
    freeaddrinfo(res);
    close(fd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
