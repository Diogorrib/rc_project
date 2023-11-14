#include "user.h"

char *as_ip = ""; //idk what to do here
char *as_port = PORT;

void filter_input(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-n"))
            as_ip = argv[i+1];

        else if (!strcmp(argv[i], "-p"))
            as_port = argv[i+1];   
    }
}

int main(int argc, char **argv) {
    if (argc > 1) 
        filter_input(argc, argv);
}
