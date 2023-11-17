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

int main(int argc, char **argv) {
    filter_input(argc, argv);
}
