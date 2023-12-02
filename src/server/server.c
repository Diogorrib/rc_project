#include "server.h"
#include "file_creation.h"
#include "process_server.h"
#include "parse_server.h"
#include <sys/wait.h>

int verbose_mode = 0; // if zero verbose mode is off else is on
char *as_port = DEFAULT_PORT;

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

void myauctions(char *buffer, char *msg) { printf("TODO: myauctions"); (void)buffer; (void)msg; }
void mybids(char *buffer, char *msg) { printf("TODO: mybids"); (void)buffer; (void)msg; }
void list(char *buffer, char *msg) { printf("TODO: list"); (void)buffer; (void)msg; }
void show_record(char *buffer, char *msg) { printf("TODO: show_record"); (void)buffer; (void)msg; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// UDP /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    else if (!strcmp("LST ", cmd))
        list(buffer, msg);
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
            printf("ERR: TCP: send timeout\n");
            return -1;
        }
        nread=read(fd,ptr,(size_t)nleft);
        if(nread <= 0) break;   // closed by user or timeout event
        nleft-=nread; ptr+=nread;
    }
    return 0;
}

void open_auction(int fd) { (void)fd; printf("TODO: open_auction\n"); }
void close_auction(int fd) { (void)fd; printf("TODO: close_auction\n"); }
void show_asset(int fd) { (void)fd; printf("TODO: show_asset\n"); }
void bid(int fd) { (void)fd; printf("TODO: bid\n"); }

void parse_tcp_buffer(int fd) {
    char cmd[CMD_N_SPACE+1];
    memset(cmd, '\0', CMD_N_SPACE+1);
    if (read_from_tcp(fd, cmd, CMD_N_SPACE) == -1)
        return;

    /* Compare cmd with the list of possible udp actions */
    if (!strcmp("OPA ", cmd))
        open_auction(fd);
    else if (!strcmp("CLS ", cmd))
        close_auction(fd);
    else if (!strcmp("SAS ", cmd))
        show_asset(fd);
    else if (!strcmp("BID ", cmd))
        bid(fd);
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
    char host[NI_MAXHOST], service[NI_MAXSERV];

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

                    parse_tcp_buffer(new_fd);

                    errcode=getnameinfo((struct sockaddr*)&addr,addrlen,host,sizeof host,service,sizeof service,0);
                    /* if(errcode==0)
                        request_received(NULL,host,service,verbose_mode);
                    else {
                        printf("ERR: TCP: getnameinfo\n");
                        close(new_fd); freeaddrinfo(res); close(fd); return;
                    } */
                    
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
