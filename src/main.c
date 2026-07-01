#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include "../include/server.h"

volatile sig_atomic_t keep_running = 1; // initializing the main while loop control variable.


int main(void) {
    if (mapper_custom_sig_handlers(SIGINT, handle_shutdown) == -1) {
        perror("Cannot map the SIGINT handler");
        exit(3);
    }
    const int server_status = accept_connections();
    if (server_status == -1) {
        printf("Server shutdown due to error\n");
        return 1;
    }
    printf("Server shutdown gracefully.\n");
    return 0;
}