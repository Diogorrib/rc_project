#include "user.h"

char *as_ip = DEFAULT_IP; //idk what to do here
char *as_port = DEFAULT_PORT;

void filter_input(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-n"))
            as_ip = argv[i+1];

        else if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];   
    }
}

void login(char *buffer) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char status[4];
    char uid[7];
    char password[9];

    (void) buffer; //just to troll the termainal

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd==-1) //error
        exit(1);
    
    memset(&hints, 0, sizeof hints); 
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;
    
    errcode = getaddrinfo(as_ip, as_port, &hints, &res);
    if (errcode!=0) //error 
        exit(1);
    
    n = sendto(fd, "LIN", 3, 0, res->ai_addr, res->ai_addrlen); 
    if (n==-1) //error
        exit(1);
    n = sendto(fd, uid, 6, 0, res->ai_addr, res->ai_addrlen); 
    if (n==-1) //error
        exit(1);
    n = sendto(fd, password, 8, 0, res->ai_addr, res->ai_addrlen); 
    if (n==-1) //error
        exit(1);
    
    addrlen = sizeof(addr);
    n = recvfrom(fd, status, 3, 0, (struct sockaddr*) &addr, &addrlen);
    if (n==-1) //error
        exit(1);
    
    n = write(1, status, 3);
    
    freeaddrinfo(res);
    close(fd);
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
            //login(input_buffer);
            printf("A\n");
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
