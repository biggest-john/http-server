#include <stdio.h>
#include <unistd.h>
#include "../include/server.h"

int main(void) {
    printf("Initializing core socket server...\n");

    int listen_fd = create_listening_socket();
    if (listen_fd == -1) {
        fprintf(stderr, "Failed to initialize listening socket.\n");
        return 1;
    }

    printf("Server successfully listening on port %s!\n", PORT);
    printf("Socket File Descriptor allocated by kernel: %d\n", listen_fd);

    // For now, we immediately close it and exit safely
    close(listen_fd);
    printf("Socket closed. Clean exit.\n");

    return 0;
}