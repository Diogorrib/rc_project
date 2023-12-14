#include "parse_user.h"
#include "process_user.h"
#include "user.h"

char *as_ip = DEFAULT_IP;
char *as_port = DEFAULT_PORT;
char uid[UID+1], password[PASSWORD+1];
char input_buffer[BUFSIZ];
int logged_in = 0;  // if logged in 1, if not logged in 0

void filter_input(int argc, char **argv) {
    if (argc == 1) // only one argument no need for updates
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

//////////////////////////////////////// HELPER FUNCTIONS TO CARRY OUT THE ////////////////////////////////////////////
//////////////////////////////////////// OPERATIONS GIVEN BY THE COMMANDS /////////////////////////////////////////////

int no_uid_pass(char *command) {
    /* there is no UID or password on the user app */
    if (strlen(uid) != UID || strlen(password) != PASSWORD) {
        printf("incorrect %s attempt. Login first\n", command);
        return 1;
    }
    return 0;
}

void user_login(const char *aux_uid, const char *aux_pass) {
    strcpy(uid, aux_uid);       // store uid in the global variable
    strcpy(password, aux_pass); // store password in the global variable
    logged_in = 1;
}

//////////////////////////////////////// COMMUNICATION WITH THE SERVER ////////////////////////////////////////////////

int udp(char *buffer, size_t size, char *msg_received) { 
    struct addrinfo hints,*res;
    int fd,errcode;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n;

    fd=socket(AF_INET,SOCK_DGRAM,0);    // UDP socket
    if(fd==-1) {                        // error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;        // IPv4
    hints.ai_socktype=SOCK_DGRAM;   // UDP socket

    errcode=getaddrinfo(as_ip,as_port,&hints,&res);
    if(errcode!=0) {    // error
        printf("Can't connect with the server AS. Try again\n");
        close(fd); return -1;
    }

    if (set_send_timeout(fd, USER_TIMEOUT) == -1) {
        printf("Can't connect with the server AS. Try again\n");
        freeaddrinfo(res); close(fd); return -1;
    }

    /* send message to AS */
    n=sendto(fd,buffer,strlen(buffer),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) { // error
        printf("Can't send to server AS. Try again\n");
        freeaddrinfo(res); close(fd); return -1;
    }

    if (set_recv_timeout(fd, USER_TIMEOUT) == -1) {
        printf("Can't connect with the server AS. Try again\n");
        freeaddrinfo(res); close(fd); return -1;
    }

    addrlen=sizeof(struct sockaddr_in);
    /* receive message from AS */
    n=recvfrom(fd,msg_received,size,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) { // error
        printf("Can't receive from server AS. Try again\n");
        freeaddrinfo(res); close(fd); return -1;
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
    if (sigaction(SIGPIPE,&act,NULL) == -1) {   // error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    fd=socket(AF_INET,SOCK_STREAM,0);   // TCP socket
    if(fd == -1) {                      // error
        printf("Can't connect with the server AS. Try again\n");
        return -1;
    }

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET ;       // IPv4
    hints.ai_socktype=SOCK_STREAM;  // TCP socket

    n=getaddrinfo(as_ip,as_port,&hints,&res);
    if(n != 0) {    // error
        printf("Can't connect with the server AS. Try again\n");
        close(fd); return -1;
    }

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n == -1) {   // error
        printf("Can't connect with the server AS. Try again\n");
        freeaddrinfo(res); close(fd); return -1;
    }

    nleft=(ssize_t)strlen(msg_sent); ptr=msg_sent;
    /* send message to AS not inclunding the \n */
    while (nleft>0){if(set_send_timeout(fd, USER_TIMEOUT) == -1) {
                        printf("Can't send to server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return -1;
                    }
                    nwritten=write(fd,ptr,(size_t)nleft);
                    if(nwritten <= 0){//error
                        printf("Can't send to server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return -1;}
                    nleft-=nwritten; ptr+=nwritten;}  

    if (fname != NULL && fname[0] != '\0') {
        struct stat file_stat;
        /* Open file for reading */
        int file = open(fname, O_RDONLY);
        if (file == -1) {
            printf("Error opening file %s for reading\n", fname);
            freeaddrinfo(res); close(fd); return -1;
        }
        /* Get file stats */
        if (fstat(file, &file_stat) < 0) {
            printf("Error opening file %s for reading\n", fname);
            close(file); freeaddrinfo(res); close(fd); return -1;
        }
        long offset = 0;
        long remain_data = file_stat.st_size;
        ssize_t sent_bytes;
        /* Sending file data */
        while (((sent_bytes = sendfile(fd, file, &offset, BUFSIZ)) > 0) && (remain_data > 0)) {
            if(set_send_timeout(fd, USER_TIMEOUT) == -1) {
                printf("Error sending file %s\n", fname);
                close(file); freeaddrinfo(res); close(fd); return -1;
            }
            remain_data -= sent_bytes;
        }
        close(file);
    }
    /* send \n to AS */
    ptr = "\n"; nwritten=write(fd,ptr,1);
    nleft=size; ptr=msg_received;
    /* receive message from AS */
    while (nleft>0){if(set_recv_timeout(fd, USER_TIMEOUT) == -1) {
                        printf("Can't receive from server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return -1;
                    }
                    nread=read(fd,ptr,(size_t)nleft);
                    if(nread == -1){//error
                        printf("Can't receive from server AS. Try again\n");
                        freeaddrinfo(res); close(fd); return -1;}
                    else if(nread == 0) break; //closed by peer
                    nleft-=nread; ptr+=nread;}
    if (fname != NULL && fname[0] == '\0') {
        if (process_sa(msg_received, fd, fname) == -1) {
            freeaddrinfo(res); close(fd); return -1;
        }
    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}

//////////////////////////////////// THE FOLLOWING FUNCTIONS ARE USED TO CARRY OUT THE ////////////////////////////////
/////////////////////////// OPERATIONS GIVEN BY THE COMMANDS REFERRED TO IN THE PROJECT STATEMENT /////////////////////

void login() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];
    char aux_uid[UID+1], aux_pass[PASSWORD+1];

    /* parsing of the all string received in the command line */
    if (confirm_login_input(input_buffer, aux_uid, aux_pass) == -1) 
        return;

    if (logged_in) { // only one user can be logged in per user app
        printf("%s already logged in. Please logout to use another user or to login again\n", uid);
        return;
    }

    memset(buffer, '\0', LOGIN_SND);        // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LOGIN_RCV);  // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "LIN", aux_uid, aux_pass);

    if (udp(buffer, LOGIN_RCV, msg_received) == -1) // UDP connection with the server
        return;

    if (process_login(msg_received, aux_uid))   // analyse the message received
        user_login(aux_uid, aux_pass);
}

void logout() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];

    /* parsing of the all string received in the command line */
    if (confirm_only_cmd_input(input_buffer, "logout") == -1)
        return;

    /* verification of the existence of UID and password */
    if (no_uid_pass("logout")) return;

    memset(buffer, '\0', LOGIN_SND);        // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LOGIN_RCV);  // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "LOU", uid, password);

    if (udp(buffer, LOGIN_RCV, msg_received) == -1) // UDP connection with the server
        return;  

    if (process_logout(msg_received, uid))  // analyse the message received
        logged_in = 0;
}

void unregister() {
    char buffer[LOGIN_SND];
    char msg_received[LOGIN_RCV];

    /* parsing of the all string received in the command line */
    if (confirm_only_cmd_input(input_buffer, "unregister") == -1)
        return;
    
    /* verification of the existence of UID and password */
    if (no_uid_pass("unregister")) return;

    memset(buffer, '\0', LOGIN_SND);        // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LOGIN_RCV);  // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s\n", "UNR", uid, password);

    if (udp(buffer, LOGIN_RCV, msg_received) == -1) // UDP connection with the server
        return;

    if (process_unregister(msg_received, uid))  // analyse the message received
        logged_in = 0;
}

void open_auction() {
    char msg_received[OPEN_RCV];
    char name[NAME+1], fname[FNAME+1];
    char buffer[OPEN_SND];
    int start_value, timeactive;
    long fsize;

    /* parsing of the all string received in the command line */
    if (confirm_open_input(input_buffer, name, fname, &start_value, &timeactive) == -1)
        return;

    /* verification of the existence of UID and password */
    if (no_uid_pass("open")) return;

    if (get_file_size(fname, &fsize) == -1) // Bytes needed to the file
        return;
    
    memset(buffer, '\0', OPEN_SND);         // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', OPEN_RCV);   // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s %s %d %d %s %ld ",
            "OPA", uid, password, name, start_value, timeactive, fname, fsize);

    if (tcp(buffer, fname, OPEN_RCV, msg_received) == -1)   // TCP connection with the server
        return;

    process_open(msg_received); // analyse the message received
}

void close_auction() {
    char buffer[CLOSE_SND];
    char msg_received[CLS_RCV];
    char aid[AID+1];

    /* parsing of the all string received in the command line */
    if (confirm_aid_input(input_buffer, "close", aid) == -1)
        return;

    /* verification of the existence of UID and password */
    if (no_uid_pass("close")) return;

    memset(buffer, '\0', CLOSE_SND);        // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', CLS_RCV);    // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s %s", "CLS", uid, password, aid);

    if (tcp(buffer, NULL, CLS_RCV, msg_received) == -1) // TCP connection with the server
        return;

    process_close(msg_received, aid, uid);  // analyse the message received
}

void myauctions(char *first_word){
    char msg_received[LST_RCV];
    char buffer[MY_SND];

    /* parsing of the all string received in the command line */
    if (confirm_only_cmd_input(input_buffer, first_word) == -1)
        return;

    /* verification of the existence of UID and password */
    if (no_uid_pass("myauctions")) return;

    memset(buffer, '\0', MY_SND);           // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LST_RCV);    // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s\n", "LMA", uid);
    if (udp(buffer, LST_RCV, msg_received) == -1)   // UDP connection with the server
        return;
        
    process_ma(msg_received);   // analyse the message received
}

void mybids(char *first_word){ 
    char msg_received[LST_RCV];
    char buffer[MY_SND];

    /* parsing of the all string received in the command line */
    if (confirm_only_cmd_input(input_buffer, first_word) == -1)
        return;

    /* verification of the existence of UID and password */
    if (no_uid_pass("mybids")) return;

    memset(buffer, '\0', MY_SND);           // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LST_RCV);    // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s\n", "LMB", uid);
    if (udp(buffer, LST_RCV, msg_received) == -1)   // UDP connection with the server
        return;
        
    process_mb(msg_received);   // analyse the message received
}

void list(char *first_word) {
    char msg_received[LST_RCV];
    char buffer[CMD_N_SPACE+1];

    /* parsing of the all string received in the command line */
    if (confirm_only_cmd_input(input_buffer, first_word) == -1)
        return;

    memset(buffer, '\0', CMD_N_SPACE+1);    // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', LST_RCV);    // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s\n", "LST");
    if (udp(buffer, LST_RCV, msg_received) == -1)   // UDP connection with the server
        return;

    process_list(msg_received); // analyse the message received

}

void show_asset(char *first_word){
    char msg_received[SA_RCV];
    char buffer[SHOW_SND];
    char fname[FNAME+1], aid[AID+1];

    /* parsing of the all string received in the command line */
    if (confirm_aid_input(input_buffer, first_word, aid) == -1)
        return;

    memset(buffer, '\0', SHOW_SND);         // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', SA_RCV);     // initialize the message to be received with \0 in every index
    memset(fname, '\0', FNAME+1);           // initialize the name of the file with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s", "SAS", aid);

    /* process_sa will be called during tcp connection to receive the file */
    if (tcp(buffer, fname, SA_RCV-1, msg_received) == -1)   // TCP connection with the server
        return;
}


void bid(char *first_word){
    char msg_received[BID_RCV];
    char buffer[BID_SND]; 
    char aid[AID+1], bid_value[MAX_4_SOME_INTS+1];

    /* parsing of the all string received in the command line */
    if(confirm_bid_input(input_buffer, first_word, aid, bid_value) == -1){
        return;
    }

    /* verification of the existence of UID and password */
    if (no_uid_pass("bid")) return;

    memset(buffer, '\0', BID_SND);          // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', BID_RCV);    // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s %s %s %s", "BID", uid, password, aid, bid_value);

    if (tcp(buffer, NULL, BID_RCV, msg_received) == -1) // TCP connection with the server
        return;

    process_bid(msg_received, aid); // analyse the message received
}

void show_record(char *first_word){
    char msg_received[SR_RCV];
    char buffer[SHOW_SND];
    char aid[AID+1];

    /* parsing of the all string received in the command line */
    if (confirm_aid_input(input_buffer, first_word, aid) == -1)
        return;

    memset(buffer, '\0', SHOW_SND);     // initialize the message to be sent with \0 in every index
    memset(msg_received, '\0', SR_RCV); // initialize the message to be received with \0 in every index

    /* Create the message to send to AS */
    sprintf(buffer, "%s %s\n", "SRC", aid);

    if (udp(buffer, SR_RCV, msg_received) == -1)
        return;

    process_sr(msg_received, aid);
}

///////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    char first_word[FIRST_WORD];

    filter_input(argc, argv);

    /* UID and password initialized as empty string to make requests */
    memset(uid, '\0', UID+1);
    memset(password, '\0', PASSWORD+1);

    while(1) {
        /* Get a line from the input terminal */
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            printf("Command not valid\n");
            continue;
        }
        memset(first_word, '\0', FIRST_WORD); // does not save the string from previous fgets call
        sscanf(input_buffer, "%15s", first_word);

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
                printf("please logout first\n");
            else break;
        }
        else if (!strcmp("open", first_word))
            open_auction();
        else if (!strcmp("close", first_word))
            close_auction();
        else if (!strcmp("myauctions", first_word) || !strcmp("ma", first_word))
            myauctions(first_word);
        else if (!strcmp("mybids", first_word) || !strcmp("mb", first_word))
            mybids(first_word);
        else if (!strcmp("list", first_word) || !strcmp("l", first_word))
            list(first_word);
        else if (!strcmp("show_asset", first_word) || !strcmp("sa", first_word))
            show_asset(first_word);
        else if (!strcmp("bid", first_word) || !strcmp("b", first_word))
            bid(first_word);
        else if (!strcmp("show_record", first_word) || !strcmp("sr", first_word))
            show_record(first_word);
        else
            printf("Command '%s' not valid\n", first_word);
    }
    return 0;
}
