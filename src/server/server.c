#include "server.h"

int verbose_mode = 0;   // if zero verbose mode is off else is on
char *as_port = DEFAULT_PORT;

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
    char ptr[100];
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;

    (void)nwritten;
    (void)n;
    (void)send_timeout;

    char host[300], service[1200];  // TOREMOVE

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
                printf("\n ---------------UDP timeout event-----------------\n");
                break;
            case -1:
                printf("ERR: UDP: select\n");
                freeaddrinfo(res); close(fd); return;
            default:
                if(FD_ISSET(fd,&testfds)) {
                    addrlen = sizeof(addr);
                    /* Set receive timeout */
                    recv_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
                    recv_timeout.tv_usec = 0;
                    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) < 0) {
                        printf("Can't connect with the server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return;
                    }
                    nread=recvfrom(fd,ptr,80,0,(struct sockaddr *)&addr,&addrlen);
                    if(nread>0) {
                        if(strlen(ptr)>0)
                            ptr[nread-1]=0;
                        printf("---UDP socket: %s\n",ptr);
                        errcode=getnameinfo( (struct sockaddr *) &addr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n",host,service);

                    }
                    /* Set send timeout */ 
                    /* send_timeout.tv_sec = UDP_TIMEOUT; // 5 seconds timeout
                    send_timeout.tv_usec = 0;
                    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout)) < 0) {
                        printf("Can't connect with the server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return -1;
                    } */
                }
                
        }
    }
    freeaddrinfo(res);
    close(fd);
}

void tcp() {
    fd_set inputs, testfds;
    struct timeval timeout;
    int fd,n,out_fds;
    ssize_t nleft,nread,nwritten;
    char *ptr;
    char buffer[100];
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    socklen_t addrlen;

    (void)nwritten;

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
                printf("\n ---------------TCP timeout event-----------------\n");
                break;
            case -1:
                printf("ERR: UDP: select\n");
                freeaddrinfo(res); close(fd); return;
            default:
                if(FD_ISSET(fd,&testfds)) {
                    int new_fd;
                    addrlen = sizeof(addr);
                    if((new_fd=accept(fd, (struct sockaddr*) &addr, &addrlen))==-1) {
                        printf("ERR: TCP: accept\n"); close(fd); return;
                    }

                    nleft=80; ptr=buffer;
                    while (nleft>0){nread=read(new_fd,ptr,(size_t)nleft);
                                    if(nread == -1){//error
                                        printf("Can't receive from server AS. Try again\n");
                                        freeaddrinfo(res); close(new_fd); close(fd); return;}
                                    else if(nread == 0) break; //closed by peer
                                    nleft-=nread; ptr+=nread;}

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

    switch (childPid = fork()) {
    case -1:
        printf("ERR: creating child process");
        return -1;

    case 0: // udp process
        udp();
        break;

    default: // tcp process
        tcp();
    }
    return 0;
}
