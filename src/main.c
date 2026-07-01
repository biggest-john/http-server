#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../include/server.h"

int main(void) {
    printf("Initializing core socket server...\n");

    const int listen_fd = create_listening_socket();
    if (listen_fd == -1) {
        fprintf(stderr, "Failed to initialize listening socket.\n");
        return 1;
    }
    printf("Server successfully listening on port %s!\n", PORT);

    while (1) {
        struct sockaddr_storage client;
        socklen_t client_size  = sizeof(client);

        const int client_fd = accept(listen_fd, (struct sockaddr *) &client, &client_size);

        if (client_fd == -1) {
            perror("client_fd");
            return 1;
        }

        handle_client_echo(client_fd);
    }
    close(listen_fd); // this is unreachable, would implement graceful shutdown soon.
    return 0;
}