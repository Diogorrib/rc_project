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
    struct timeval timeout;

    int out_fds,errcode;
    ssize_t ret, idk;

    char prt_str[90];

    struct addrinfo hints, *res;
    struct sockaddr_in udp_useraddr;
    socklen_t addrlen;
    int fd;

    char host[300], service[1200];

    (void)idk;


// UDP SERVER SECTION
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    if((errcode=getaddrinfo(NULL,as_port,&hints,&res))!=0)
        exit(1);// On error

    fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(fd==-1)
        exit(1);

    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)
    {
        sprintf(prt_str,"Bind error UDP server\n");
        idk = write(1,prt_str,strlen(prt_str));
        exit(1);// On error
    }
    if(res!=NULL)
        freeaddrinfo(res);

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(fd,&inputs); // Set UDP channel on

//    printf("Size of fd_set: %d\n",sizeof(fd_set));
//    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    printf("UDP_started\n");

    while(1)
    {
        testfds=inputs; // Reload mask
//        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=5;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
// testfds is now '1' at the positions that were activated
//        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        switch(out_fds)
        {
            case 0:
                printf("\n ---------------Timeout event-----------------\n");
                break;
            case -1:
                perror("select");
                exit(1);
            default:
                if(FD_ISSET(fd,&testfds))
                {
                    addrlen = sizeof(udp_useraddr);
                    ret=recvfrom(fd,prt_str,80,0,(struct sockaddr *)&udp_useraddr,&addrlen);
                    if(ret>0)
                    {
                        if(strlen(prt_str)>0)
                            prt_str[ret-1]=0;
                        printf("---UDP socket: %s\n",prt_str);
                        errcode=getnameinfo( (struct sockaddr *) &udp_useraddr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n",host,service);

                    }
                }
        }
    }
    freeaddrinfo(res);
    close(fd);
}

void tcp() {
    fd_set inputs, testfds;
    struct timeval timeout;

    int out_fds,errcode;
    ssize_t ret, idk;

    char prt_str[90];

    struct addrinfo hints, *res;
    struct sockaddr_in tcp_useraddr;
    socklen_t addrlen;
    int fd;

    char host[300], service[1200];

    (void)idk;


// TCP SERVER SECTION
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE|AI_NUMERICSERV;

    if((errcode=getaddrinfo(NULL,as_port,&hints,&res))!=0)
        exit(1);// On error

    fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(fd==-1)
        exit(1);

    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)
    {
        sprintf(prt_str,"Bind error UDP server\n");
        idk = write(1,prt_str,strlen(prt_str));
        exit(1);// On error
    }
    if(res!=NULL)
        freeaddrinfo(res);

    if(listen(fd,5) == -1) exit(1);

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(fd,&inputs); // Set TCP channel on

//    printf("Size of fd_set: %d\n",sizeof(fd_set));
//    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    printf("TCP_started\n");

    while(1)
    {
        testfds=inputs; // Reload mask
//        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=10;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
// testfds is now '1' at the positions that were activated
//        printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        switch(out_fds)
        {
            case 0:
                printf("\n ---------------Timeout event-----------------\n");
                break;
            case -1:
                perror("select");
                exit(1);
            default:
                if(FD_ISSET(fd,&testfds))
                {
                    int new_fd;
                    addrlen = sizeof(tcp_useraddr);
                    if((new_fd=accept(fd, (struct sockaddr*) &tcp_useraddr, &addrlen))==-1)
                        exit(1);
                    ret=read(new_fd,prt_str,10);
                    if(ret>0)
                    {
                        if(strlen(prt_str)>0)
                            prt_str[ret-1]=0;
                        printf("---TCP socket: %s\n",prt_str);
                        errcode=getnameinfo( (struct sockaddr *) &tcp_useraddr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n",host,service);

                    }
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
        break;
    }

    return 0;
}
