#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#define PORT "8081"
#define MAX_CONNECTIONS 10
#define BACKLOG 10
#define BUFFER_SIZE 1024

int main() {
    // Array of structures to monitor
    struct pollfd fds[MAX_CONNECTIONS] = {0};

    int listen_fd, new_fd;
    struct addrinfo hints, *res;
    struct sockaddr_storage client_storage;
    socklen_t client_size;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo error");
        exit(1);
    }
    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind error");
        exit(1);
    }
    freeaddrinfo(res);

    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listening failed");
        exit(1);
    }

    printf("Server successfully spinning on port %s...\n", PORT);

    // 1. Explicitly initialize our tracking array slots to -1 (meaning empty)
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        fds[i].fd = -1;
    }

    // 2. Put the listener strictly in slot 0
    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;
    int num_fds = 1; // Tracks the current count of structures active in fds[]

    while (1) {
        // Hand our true count (num_fds) to poll
        int poll_count = poll(fds, num_fds, -1);
        if (poll_count < 0) {
            perror("poll error");
            exit(1);
        }

        // 3. START AT INDEX 0!
        for (int i = 0; i < num_fds; i++) {

            // Skip slots that have no active returned events
            if (fds[i].revents == 0) continue;

            // CASE A: The Listener triggered
            if (fds[i].fd == listen_fd) {
                if (fds[i].revents & POLLIN) {
                    client_size = sizeof(client_storage);
                    new_fd = accept(listen_fd, (struct sockaddr *)&client_storage, &client_size);

                    if (new_fd < 0) {
                        perror("accept error");
                    } else {
                        // Check if we have room for another client
                        if (num_fds < MAX_CONNECTIONS) {
                            // Target the fresh slot at the END of our active count boundary
                            fds[num_fds].fd = new_fd;
                            fds[num_fds].events = POLLIN; // CRITICAL: Tell poll to watch it!
                            num_fds++; // Expand our watch boundary safely
                            printf("[Server] Connection accepted on fd %d\n", new_fd);
                        } else {
                            printf("[Server] Max connections reached! Rejecting client.\n");
                            close(new_fd);
                        }
                    }
                }
            }
            // CASE B: An existing client triggered
            else if (fds[i].revents & POLLIN) {
                char buffer[BUFFER_SIZE] = {0};
                int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                if (bytes_received <= 0) {
                    printf("[Server] Socket %d disconnected.\n", fds[i].fd);
                    close(fds[i].fd);

                    // 4. FIX FRAGMENTATION: Swap the disconnected client
                    // with the very last element in our active list
                    fds[i] = fds[num_fds - 1];
                    num_fds--; // Shrink the active boundary count
                    i--;       // Decrement our loop index so we scan the swapped element next!
                }
                else {
                    printf("[Socket %d sent]: %s", fds[i].fd, buffer);
                    send(fds[i].fd, buffer, bytes_received, 0);
                }
            }
        }
    }
    return 0;
}