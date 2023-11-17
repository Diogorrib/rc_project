#include "user.h"

char *as_ip = DEFAULT_IP;
char *as_port = DEFAULT_PORT;
char uid[UID+1];
char password[PASSWORD+1];
int logged_in = 0;  // if logged in 1, if not logged in 0

void filter_input(int argc, char **argv) {
    if (argc > 1) // only one argument no need for updates
        return;
    for (int i = 1; i < argc; i++) {
        /* update the ip to use during this user app */
        if (!strcmp(argv[i], "-n"))
            as_ip = argv[i+1];
        /* update the port to use during this user app */
        else if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];
    }
}

int udp(char *buffer, size_t size, char *msg_received) { 
    struct addrinfo hints,*res;
    int fd,errcode;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_DGRAM;//UDP socket

    errcode=getaddrinfo(as_ip,as_port,&hints,&res);
    if(errcode!=0) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    /* send message to AS */
    n=sendto(fd,buffer,strlen(buffer),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    addrlen=sizeof(addr);
    /* receive message from AS */
    n=recvfrom(fd,msg_received,size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }
    
    freeaddrinfo(res);
    close(fd);
    return 0;
}

void user_login(const char **login_data) {
    strcpy(uid, login_data[0]);
    strcpy(password, login_data[1]);
    logged_in = 1;
}

void login() {
    char msg_received[LOGIN_MSG];
    const char *login_data[2];
    char *buffer;
    char command[CMD_N_SPACE+1];
    char status[5];

    if (logged_in) { // only one user can be logged in per user app
        printf("%s already logged in. Please logout to use another user or login again\n", uid);
        return;
    }

    /* read arguments uid and password from the login command */
    login_data[0] = strtok(NULL, "\n");
    login_data[1] = strtok(NULL, "\n");
    buffer = build_string("LIN", login_data, 2);
    if(strlen(login_data[0]) != 6 || strlen(login_data[1]) != 8){
        printf("incorrect login attempt\n");
        return;
    }

    msg_received[7] = '\0';
    msg_received[8] = '\0';
    if (udp(buffer, LOGIN_MSG, msg_received) == -1)
        return;
    strncpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    strncpy(status, msg_received+4, 5);

    if(strcmp(command, "RLI ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "REG\n")){
        printf("new %s registered\n", login_data[0]);
        user_login(login_data);
    }
    else if(!strcmp(status, "NOK\n"))
        printf("incorrect login attempt\n");
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0'){
        printf("successful login\n");
        user_login(login_data);
    }
    else printf("%s", msg_received);
}

void logout() {
    char msg_received[LOGIN_MSG];
    const char *login_data[2];
    char *buffer;
    char command[CMD_N_SPACE+1];
    char status[5];

    /* get arguments uid and password from user app global variables */
    login_data[0] = uid;
    login_data[1] = password;
    buffer = build_string("LOU", login_data, 2);

    msg_received[7] = '\0';
    msg_received[8] = '\0';
    if (udp(buffer, LOGIN_MSG, msg_received) == -1)
        return;
    strncpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    strncpy(status, msg_received+4, 5);

    if(strcmp(command, "RLO ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "NOK\n"))
        printf("%s not logged in\n", login_data[0]);
    else if(!strcmp(status, "UNR\n"))
        printf("unknown %s\n", login_data[0]);
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0'){
        printf("successful logout\n");
        logged_in = 0;
    }
    else printf("%s", msg_received);
}

void unregister() {
    char msg_received[LOGIN_MSG];
    const char *login_data[2];
    char *buffer;
    char command[CMD_N_SPACE+1];
    char status[5];

    /* get arguments uid and password from user app global variables */
    login_data[0] = uid;
    login_data[1] = password;
    buffer = build_string("UNR", login_data, 2);

    msg_received[7] = '\0';
    msg_received[8] = '\0';
    if (udp(buffer, LOGIN_MSG, msg_received) == -1)
        return;
    strncpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    strncpy(status, msg_received+4, 5);

    if(strcmp(command, "RUR ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "NOK\n"))
        printf("incorrect unregister attempt\n");
    else if(!strcmp(status, "UNR\n"))
        printf("unknown %s\n", login_data[0]);
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0'){
        printf("successful unregister\n");
        logged_in = 0;
    }
    else printf("%s", msg_received);
}

int confirm_list(char *msg, char *auction, int i) {
    if (auction[3] != ' ' || (auction[5] != ' ' && auction[5] != '\n') || (auction[5] == '\n' && msg[13 + (i*6)] != '\0'))
        return 1;
    for (int index = 0; index < 3; index++) {
        if (auction[index] < '0' || auction[index] > '9')
            return 1;
    }
    return ((auction[4] != '0' && auction[4] != '1') || (auction[0] == '0' && auction[1] == '0' && auction[2] == '0'));
}

void auction_to_string(char *string, char *auction) {
    char aux[16]; // 3 digits + " not active\n" + '\0'
    
    snprintf(aux, sizeof(aux), "%.*s", 4, auction); // AID + 1 space
    if (auction[4] == '0') {
        snprintf(aux+4, sizeof(aux)-4, "%.*s", 11, "not active\n");
        aux[15] = '\0';
    }
    else if (auction[4] == '1') {
        snprintf(aux+4, sizeof(aux)-4, "%.*s", 7, "active\n");
        aux[11] = '\0';
    }
    strcpy(string + strlen(string), aux);
}

void list() {
    char msg_received[LST_MSG+1];
    char *buffer;
    char command[CMD_N_SPACE+1];
    char status[4];
    char auction[7];
    char auctions[LST_MSG+1]; //mudar isto mais tarde

    buffer = build_string("LST", NULL, 0);

    for (size_t i = 0; i <= LST_MSG; i++) {
        msg_received[i] = '\0'; // initialize the string with \0 in every index
        auctions[i] = '\0';
    }
    if (udp(buffer, LST_MSG, msg_received) == -1)
        return;
    //printf("%s", msg_received);

    snprintf(command, sizeof(command), "%.*s", CMD_N_SPACE, msg_received);
    command[CMD_N_SPACE] = '\0';
    snprintf(status, sizeof(status), "%.*s", 3, msg_received+4);
    status[3] = '\0';
    auction[6] = '\0';

    if(strcmp(command, "RLS "))
        printf("%s", msg_received);
    else if(!strcmp(status, "NOK") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("no auction was yet started\n");
    else if(!strcmp(status, "OK ")){
        for (int i = 0; i < 999; i++) {
            snprintf(auction, sizeof(auction), "%.*s", 6, msg_received + 7 + (i*6));
            if (confirm_list(msg_received,auction,i)) {
                printf("%s acjaskjc", msg_received);
                return;
            }        
            auction_to_string(auctions, auction);
            if (auction[5] == '\n') break;
        }
        printf("%s", auctions);
    }
    else printf("%s", msg_received);
}

void show_record() {}

int main(int argc, char **argv) {
    char input_buffer[1024];
    char *first_word;
    
    filter_input(argc, argv);

    /* uid and password initialized as empty string to make requests */
    uid[0] = '\0';
    password[0] = '\0';

    while(1) {
        /* Get a line from the input terminal */
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            printf("ERR: Command '%s' not valid\n", first_word);
            continue;
        }
        /* Replace the spaces for \n to help using strtok function */
        for (size_t i = 0; i < strlen(input_buffer); ++i) {
            if (input_buffer[i] == ' ')
                input_buffer[i] = '\n';
        }
        first_word = strtok(input_buffer, "\n");
    
        /* Compare the first word of each input line with a possible command (action) */
        if (!strcmp("login", first_word))
            login();
        else if (!strcmp("logout", first_word))
            logout();
        else if (!strcmp("unregister", first_word))
            unregister();
        else if (!strcmp("exit", first_word)) {
            if (logged_in)
                printf("Please logout first\n");
            else break;
        }
        else if (!strcmp("open", first_word))
            printf("D\n");
        else if (!strcmp("close", first_word))
            printf("E\n");
        else if (!strcmp("myauctions", first_word) || !strcmp("ma", first_word))
            printf("F\n");
        else if (!strcmp("mybids", first_word) || !strcmp("mb", first_word))
            printf("G\n");
        else if (!strcmp("list", first_word) || !strcmp("l", first_word))
            list();
        else if (!strcmp("show_asset", first_word) || !strcmp("sa", first_word))
            printf("I\n");
        else if (!strcmp("bid", first_word) || !strcmp("b", first_word))
            printf("J\n");
        else if (!strcmp("show_record", first_word) || !strcmp("sr", first_word))
            show_record();
        else
            printf("ERR: Command '%s' not valid\n", first_word);
    }
}
