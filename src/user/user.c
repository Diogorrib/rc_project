#include "parse_user.h"
#include "user.h"

char *as_ip = DEFAULT_IP;
char *as_port = DEFAULT_PORT;
char uid[UID+1], password[PASSWORD+1];
char input_buffer[1024];
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

int tcp(char *msg_sent, char *fname, ssize_t size, char *msg_received) { 
    struct addrinfo hints,*res;
    struct sigaction act;
    ssize_t nleft,nwritten,nread;
    int fd,n;
    char *ptr;

    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;
    if (sigaction(SIGPIPE,&act,NULL) == -1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd == -1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET ;//IPv4
    hints.ai_socktype=SOCK_STREAM;//TCP socket

    n=getaddrinfo(as_ip,as_port,&hints,&res);
    if(n != 0) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    nleft=(ssize_t)strlen(msg_sent); ptr=msg_sent;
    /* send message to AS not inclunding the \n */
    while (nleft>0){nwritten=write(fd,ptr,(size_t)nleft);
                    if(nwritten <= 0){//error
                        printf("Can't connect with the server AS. Try again\n");
                        return -1;}
                    nleft-=nwritten; ptr+=nwritten;}  

    if (fname != NULL) {
        if (send_file(fd, fname) == -1)
            return -1;
    }
    ptr = "\n"; /* send \n to AS */
    nwritten=write(fd,ptr,1);
    if(nwritten <= 0) {//error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }
    nleft=size; ptr=msg_received;
    /* receive message from AS */
    while (nleft>0){nread=read(fd,ptr,(size_t)nleft);
                    if(nread == -1){//error
                        printf("Can't connect with the server AS. Try again\n");
                        return -1;}
                    else if(nread == 0) break; //closed by peer
                    nleft-=nread; ptr+=nread;}
    nread=size-nleft;
    
    freeaddrinfo(res);
    close(fd);
    return 0;    
}

void user_login(const char *aux_uid, const char *aux_pass) {
    strcpy(uid, aux_uid);
    strcpy(password, aux_pass);
    logged_in = 1;
}

int get_cmd_status(char *buffer, char *msg, char * cmd, char *status) {
    memset(msg, '\0', LOGIN_RCV); // initialize the msg with \0 in every index
    if (udp(buffer, LOGIN_RCV, msg) == -1)
        return -1;
    memcpy(cmd, msg, CMD_N_SPACE);
    cmd[CMD_N_SPACE] = '\0';
    memcpy(status, msg+CMD_N_SPACE, STATUS);
    status[STATUS] = '\0';
    return 0;
}

void login() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char aux_uid[UID+1], aux_pass[PASSWORD+1];

    if (confirm_login_input(input_buffer, aux_uid, aux_pass) == -1)
        return;

    if (logged_in) { // only one user can be logged in per user app
        printf("%s already logged in. Please logout to use another user or to login again\n", uid);
        return;
    }

    memset(buffer, '\0', LOGIN_SND); // initialize the buffer with \0 in every index
    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "LIN", aux_uid, aux_pass);

    if (get_cmd_status(buffer, msg_received, command, status) == -1)
        return;

    if(strcmp(command, "RLI ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "REG\n")) {
        printf("new %s registered\n", aux_uid);
        user_login(aux_uid, aux_pass);
    }
    else if(!strcmp(status, "NOK\n"))
        printf("incorrect login attempt\n");
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0') {
        printf("successful login\n");
        user_login(aux_uid, aux_pass);
    }
    else printf("%s", msg_received);
}

void logout() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];
    char command[CMD_N_SPACE+1], status[STATUS+1];

    if (confirm_only_cmd_input(input_buffer, "logout") == -1)
        return;
    
    memset(buffer, '\0', LOGIN_SND); // initialize the buffer with \0 in every index
    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "LOU", uid, password);

    if (get_cmd_status(buffer, msg_received, command, status) == -1)
        return;

    if(strcmp(command, "RLO ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "NOK\n"))
        printf("%s not logged in\n", uid);
    else if(!strcmp(status, "UNR\n"))
        printf("unknown %s\n", uid);
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0') {
        printf("successful logout\n");
        logged_in = 0;
    }
    else printf("%s", msg_received);
}

void unregister() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];
    char command[CMD_N_SPACE+1], status[STATUS+1];

    if (confirm_only_cmd_input(input_buffer, "unregister") == -1)
        return;

    memset(buffer, '\0', LOGIN_SND); // initialize the buffer with \0 in every index
    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "UNR", uid, password);

    if (get_cmd_status(buffer, msg_received, command, status) == -1)
        return;

    if(strcmp(command, "RUR ") || msg_received[8] != '\0')
        printf("%s", msg_received); 
    else if(!strcmp(status, "NOK\n"))
        printf("incorrect unregister attempt\n");
    else if(!strcmp(status, "UNR\n"))
        printf("unknown %s\n", uid);
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0') {
        printf("successful unregister\n");
        logged_in = 0;
    }
    else printf("%s", msg_received);
}

void append_auction(char *string, char *auction) {
    char aux[LST_PRINT+1]; // 3 digits + " not active\n" (worst case) + '\0'
    
    memcpy(aux, auction, AID+1); // AID + 1 space
    if (auction[AID+1] == '0') {
        memcpy(aux+AID+1, "not active\n", 11);
        aux[LST_PRINT] = '\0';
    }
    else if (auction[AID+1] == '1') {
        memcpy(aux+AID+1, "active\n", 7);
        aux[11] = '\0';
    }
    strcpy(string + strlen(string), aux);
}

int get_list(char *destination, char *msg) {
    char auction[7];
    auction[6] = '\0';

    for (int i = 0; i < MAX_AUCTION; i++) {
        /* get an auction (AID + 1 space + state + (1 space or \n)) from msg */
        memcpy(auction, msg + 7 + (i*6), 6);
        if (confirm_list(msg, auction, i)) {
            printf("%s", msg);
            return -1;
        }
        append_auction(destination, auction);
        if (auction[5] == '\n') break; // no more auctions
    }
    return 0;
}

void list(char *first_word) {
    char msg_received[LST_RCV+1];
    char buffer[CMD_N_SPACE+1];
    char auctions[LST_PRINT*MAX_AUCTION+1];
    char command[CMD_N_SPACE+1], status[STATUS];

    if (confirm_only_cmd_input(input_buffer, first_word) == -1)
        return;

    /* initialize strings with \0 in every index */
    memset(buffer, '\0', CMD_N_SPACE+1);
    memset(msg_received, '\0', LST_RCV+1);
    memset(auctions, '\0', LST_PRINT*MAX_AUCTION+1);

    /* Create the message to send to AS */
    sprintf(buffer, "%s\n", "LST");
    if (udp(buffer, LST_RCV, msg_received) == -1)
        return;

    memcpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    /* for next strcmp calls is needed strlen(status) = 3 */
    memcpy(status, msg_received+CMD_N_SPACE, STATUS-1);
    status[STATUS-1] = '\0';
    
    if(strcmp(command, "RLS "))
        printf("%s", msg_received);
    else if(!strcmp(status, "NOK") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("no auction was yet started\n");
    else if(!strcmp(status, "OK ")) {
        if (get_list(auctions, msg_received) == -1)
            return;
        printf("%s", auctions);
    }
    else printf("%s", msg_received);
}

void open_auction() {
    char msg_received[OPEN_RCV];
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char aid[AID+2], name[NAME+1];
    int start_value, timeactive;
    char *fname,*buffer;
    long fsize;

    fname = confirm_open_input(input_buffer, name, &start_value, &timeactive);
    if (fname == NULL)
        return;

    /* there is no uid or password on the user app */
    if (strlen(uid) != UID || strlen(password) != PASSWORD) {
        printf("incorrect open attempt\n");
        return;
    }

    if (get_file_size(fname, &fsize) == -1) {
        free(fname);
        return;
    }
    
    size_t buffer_size = OPEN_SND+strlen(fname);
    buffer = (char*)malloc(buffer_size);
    memset(msg_received, '\0', OPEN_RCV); // initialize the msg with \0 in every index
    memset(buffer, '\0', buffer_size); // initialize the buffer with \0 in every index
    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s %s %d %d %s %ld ",
            "OPA", uid, password, name, start_value, timeactive, fname, fsize);

    if (tcp(buffer, fname, OPEN_RCV, msg_received) == -1) {
        free(buffer); free(fname);
        return;
    }
    free(buffer); free(fname);

    memcpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    // for next strcmp calls is needed strlen(status) = 3 
    memcpy(status, msg_received+CMD_N_SPACE, STATUS-1);
    status[STATUS-1] = '\0';

    if(strcmp(command, "ROA "))
        printf("%s", msg_received); 
    else if(!strcmp(status, "NOK") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("auction could not be started\n");
    else if(!strcmp(status, "NLG") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("login is needed to open an auction\n");
    else if(!strcmp(status, "OK ") && confirm_open(msg_received)) {
        memcpy(aid, msg_received+CMD_N_SPACE+STATUS-1, AID+1);  // AID including \n
        aid[AID+1] = '\0';
        printf("auction started successfully with the identifier %s", aid);
    }
    else printf("%s", msg_received);
}

void close_auction() {
    char buffer[CLOSE_SND];
    char msg_received[CLS_RCV];
    char command[CMD_N_SPACE+1], status[STATUS+1];
    char aid[AID+1];

    if (confirm_close_input(input_buffer, aid) == -1)
        return;

    /* there is no uid or password on the user app */
    if (strlen(uid) != UID || strlen(password) != PASSWORD) {
        printf("incorrect close attempt\n");
        return;
    }

    memset(buffer, '\0', CLOSE_SND); // initialize the buffer with \0 in every index
    memset(msg_received, '\0', CLS_RCV); // initialize the msg with \0 in every index
    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s %s", "CLS", uid, password, aid);

    if (tcp(buffer, NULL, CLS_RCV, msg_received) == -1) 
        return;

    memcpy(command, msg_received, CMD_N_SPACE);
    command[CMD_N_SPACE] = '\0';
    // for next strcmp calls is needed strlen(status) = 3 
    memcpy(status, msg_received+CMD_N_SPACE, STATUS-1);
    status[STATUS-1] = '\0';

    if(strcmp(command, "RCL "))
        printf("%s", msg_received);
    else if(!strcmp(status, "OK\n") && msg_received[7] == '\0')
        printf("auction was successfully closed\n");
    else if(!strcmp(status, "NLG") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("login is needed to close an auction\n");
    else if(!strcmp(status, "EAU") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("auction %s does not exist\n", aid);
    else if(!strcmp(status, "EOW") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("auction is not owned by user %s\n", uid);
    else if(!strcmp(status, "END") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("auction time had already ended\n");
    else if(!strcmp(status, "NOK") && msg_received[7] == '\n' && msg_received[8] == '\0')
        printf("incorrect close attempt\n");
    else printf("%s", msg_received);
}

int main(int argc, char **argv) {
    char first_word[32];
    
    filter_input(argc, argv);

    /* uid and password initialized as empty string to make requests */
    memset(uid, '\0', UID+1);
    memset(password, '\0', PASSWORD+1);
    memset(first_word, '\0', 32);

    while(1) {
        /* Get a line from the input terminal */
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            printf("ERR: Command not valid\n");
            continue;
        }
        sscanf(input_buffer, "%31s", first_word);
    
        /* Compare the first word of each input line with a possible command (action) */
        if (!strcmp("login", first_word))
            login();
        else if (!strcmp("logout", first_word))
            logout();
        else if (!strcmp("unregister", first_word))
            unregister();
        else if (!strcmp("exit", first_word)) {
            if (confirm_only_cmd_input(input_buffer, "exit") == -1)
                continue;
            if (logged_in)
                printf("Please logout first\n");
            else break;
        }
        else if (!strcmp("open", first_word))
            open_auction();
        else if (!strcmp("close", first_word))
            close_auction();
        else if (!strcmp("myauctions", first_word) || !strcmp("ma", first_word))
            printf("F\n");
        else if (!strcmp("mybids", first_word) || !strcmp("mb", first_word))
            printf("G\n");
        else if (!strcmp("list", first_word) || !strcmp("l", first_word))
            list(first_word);
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
