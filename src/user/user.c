#include "user.h"

char *as_ip = DEFAULT_IP; //idk what to do here
char *as_port = DEFAULT_PORT;
int loged_in = 0;

void filter_input(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-n"))
            as_ip = argv[i+1];

        else if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];   
    }
}

void udp(char *buffer, size_t size, char *msg_received){ 
    struct addrinfo hints,*res;
    int fd,errcode;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)//error
        exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_DGRAM;//UDP socket
    errcode=getaddrinfo(as_ip,as_port,&hints,&res);
    if(errcode!=0)//error
        exit(1);
    n=sendto(fd,buffer,strlen(buffer),0,res->ai_addr,res->ai_addrlen);
    if(n==-1)//error
        exit(1);
    addrlen=sizeof(addr);
    n=recvfrom(fd,msg_received,size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1)//error
        exit(1);
    freeaddrinfo(res);
    close(fd);
}

void login() {
    char msg_received[9];
    const char *login_data[2];
    ssize_t n;
    char command[4];
    (void)n;

    login_data[0] = strtok(NULL, "\n");
    login_data[1] = strtok(NULL, "\n");
    char *buffer = build_string("LIN", login_data, 2);

    udp(buffer, 9, msg_received);
    strncpy(command, msg_received, 3);
    command[3] = '\0';
    if(strcmp(command, "RLI") || msg_received[3] != ' ')
        printf("%s\n", msg_received);
    else if(msg_received[4] == 'R' && msg_received[5] == 'E' && msg_received[6] == 'G' && msg_received[7] == '\n')
        printf("new %s registered\n", login_data[0]);
    else if(msg_received[4] == 'N' && msg_received[5] == 'O' && msg_received[6] == 'K' && msg_received[7] == '\n')
        printf("incorrect login attempt\n");
    else if(msg_received[4] == 'O' && msg_received[5] == 'K' && msg_received[6] == '\n')
        printf("successful login\n");
    else
        printf("%s\n", msg_received);
}

int main(int argc, char **argv) {
    char input_buffer[1024];
    
    if (argc > 1) 
        filter_input(argc, argv);

    while(1) {
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL)
            exit(1);
        for (size_t i = 0; i < strlen(input_buffer); ++i) {
            if (input_buffer[i] == ' ')
                input_buffer[i] = '\n';
        }
        char *first_word = strtok(input_buffer, "\n");
    
        if (!strcmp("login", first_word))
            login();
        else if (!strcmp("logout", first_word))
            printf("B\n");
        else if (!strcmp("unregister", first_word))
            printf("C\n");
        else if (!strcmp("exit", first_word))
            //check if user is logged out
            break;
        else if (!strcmp("open", first_word))
            printf("D\n");
        else if (!strcmp("close", first_word))
            printf("E\n");
        else if (!strcmp("myauctions", first_word) || !strcmp("ma", first_word))
            printf("F\n");
        else if (!strcmp("mybids", first_word) || !strcmp("mb", first_word))
            printf("G\n");
        else if (!strcmp("list", first_word) || !strcmp("l", first_word))
            printf("H\n");
        else if (!strcmp("show_asset", first_word) || !strcmp("sa", first_word))
            printf("I\n");
        else if (!strcmp("bid", first_word) || !strcmp("b", first_word))
            printf("J\n");
        else if (!strcmp("show_record", first_word) || !strcmp("sr", first_word))
            printf("K\n");
        else
            printf("ERR: Command '%s' not valid\n", first_word);
    }
}
