#include "server.h"
#include "file_creation.h"
#include "process_server.h"
#include <sys/wait.h>

int verbose_mode = 0;   // if zero verbose mode is off else is on
char *as_port = DEFAULT_PORT;

void login(char *buffer, char *msg) {
    char uid[UID+1];
    char pass[PASSWORD+1];

    /* verify if the string has the correct size */
    if (strlen(buffer) != LOGIN_SND-1) {
        sprintf(msg, "ERR\n");
        return;
    }
    /* verify if spaces are placed correctly */
    if(buffer[CMD_N_SPACE+UID] != ' ' || buffer[LOGIN_SND-2] != '\n') {
        sprintf(msg, "ERR\n");
        return;
    }
    
    memset(uid, '\0', UID+1); // initialize the uid with \0 in every index
    memset(pass, '\0', PASSWORD+1); // initialize the pass with \0 in every index
    memcpy(uid, buffer+CMD_N_SPACE, UID);
    memcpy(pass, buffer+CMD_N_SPACE+UID+1, PASSWORD);

    /* verify if the uid and pass have the correct sizes */
    if(strlen(uid) != UID || strlen(pass) != PASSWORD) {
        sprintf(msg, "ERR\n");
        return;
    }
    /* verify if the uid is only digits and the pass is only letters and digits */
    if (!is_numeric(uid) || !is_alphanumeric(pass)) {
        sprintf(msg, "ERR\n");
        return;
    }
    
    process_login(uid, pass, msg);
}

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
    /* else if (!strcmp("LOU ", cmd))
        logout(buffer);
    else if (!strcmp("UNR ", cmd))
        unregister(buffer);
    else if (!strcmp("LMA ", cmd))
        myauctions(buffer);
    else if (!strcmp("LMB ", cmd))
        mybids(buffer);
    else if (!strcmp("LST ", cmd))
        list(buffer);
    else if (!strcmp("SRC ", cmd))
        show_record(buffer); */
    else
        sprintf(msg, "ERR\n");
}

void filter_input(int argc, char **argv) {
    if (argc > 1) // only one argument no need for updates
        return;
    for (int i = 1; i < argc; i++) {
        /* update the port where AS app is running */
        if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];

        else if (!strcmp(argv[i], "-v"))
            verbose_mode = 1;
    }
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

    (void)n;
    (void)send_timeout;

    //char host[300], service[1200];  // TOREMOVE

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

                    
                    /* if(nread>0) {    // TOREMOVE
                        if(strlen(buffer)>0)
                            buffer[nread-1]=0;
                        printf("---UDP socket: %s\n",buffer);
                        errcode=getnameinfo( (struct sockaddr *) &addr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n",host,service);

                    } */
                }
                
        }
    }
    freeaddrinfo(res);
    close(fd);
}

void tcp() {
    fd_set inputs, testfds;
    struct timeval timeout,send_timeout,recv_timeout;
    int fd,n,out_fds;
    ssize_t nleft,nread,nwritten;
    char *ptr;
    char buffer[100];
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;

    (void)nwritten;
    (void)send_timeout;

    char host[300], service[1200];  // TOREMOVE


// TCP SERVER SECTION
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    n=getaddrinfo(NULL,as_port,&hints,&res);
    if(n != 0) {
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
                        printf("ERR: TCP: accept\n"); close(fd); return;
                    }

                    nleft=80; ptr=buffer;
                    while (nleft>0){
                        /* Set receive timeout */
                        recv_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
                        recv_timeout.tv_usec = 0;
                        if (setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0) {
                            printf("Can't connect with the server AS. Try again\n");
                            freeaddrinfo(res); close(fd); return;
                        }
                        nread=read(new_fd,ptr,(size_t)nleft);
                        if(nread <= 0){
                            //printf("Can't receive from server AS. Try again\n");
                            break;
                        }
                        nleft-=nread; ptr+=nread;
                    }

                    printf("---TCP socket: %s\n",buffer);
                    n=getnameinfo( (struct sockaddr *) &addr,addrlen,host,sizeof host, service,sizeof service,0);
                    if(n==0)
                        printf("       Sent by [%s:%s]\n",host,service);

                    
                    close(new_fd);
                }
        }
    }
    freeaddrinfo(res);
    close(fd);
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
