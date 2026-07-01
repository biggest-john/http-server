#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "../include/server.h"

void handle_shutdown(int sig) {
    // functon to handle the shutdown of the server ^C.
    printf("SIGINT: %i\n", sig);
    keep_running = 0; // sets main loop control to enable handling graceful shutdown
}

int mapper_custom_sig_handlers(int sig, void (*handler) (int)) {
    /*
     *custom interrupt signal handler, takes in the intended signal interrupt to be handled and then maps it to the
     * custom function(handler) the program would love to handle the specified interrupt signal.
     */
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); // to ensure that other signals are not blocked when handling current signal.
    action.sa_flags = 0;
    return sigaction(sig, &action, NULL);
}
