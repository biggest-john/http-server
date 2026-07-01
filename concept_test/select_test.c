#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#define PORT "8081"
#define BACKLOG 10
#define BUFFER_SIZE 1024

int main () {
    int listen_fd, new_fd;
    struct addrinfo hints, *res;
    struct sockaddr_storage client_storage;
    socklen_t client_size;

    fd_set master_set, ready_set;
    FD_ZERO(&master_set);
    int fd_max; // to handle the max index in the set of all input socket

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET ;// setting it to IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo error");
        exit(1);
    }

    listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        perror("setsockopt error");
    }

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
    FD_SET(listen_fd, &master_set);
    fd_max = listen_fd;

    while (1) {
        ready_set = master_set;
        if (select(fd_max +1, &ready_set, NULL, NULL, NULL) == -1) {
            perror("select exception occurred");
            exit(1);
        }
        for (int i=0; i<=fd_max; i++) {
            if (FD_ISSET(i, &ready_set)) {
                if (i == listen_fd) {
                client_size = sizeof(client_storage);
                new_fd = accept(listen_fd, (struct sockaddr *)&client_storage, &client_size);

                    if (new_fd >= 0) {
                        FD_SET(new_fd, &master_set);
                        if (new_fd > fd_max) {
                            fd_max = new_fd;
                        }
                        printf("[Server] New connection accepted on file descriptor %d\n", new_fd);
                    }
                }
                else {
                    char buffer[BUFFER_SIZE] = {0}; // Clean initialization block
                    int bytes_received = recv(i, buffer, sizeof(buffer) - 1, 0);

                    if (bytes_received <=0) {
                        printf("[Server] Socket %d disconnected.\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                    }
                    else {
                        // Process data: Echo it right back to them!
                        printf("[Socket %d sent]: %s", i, buffer);
                        send(i, buffer, bytes_received, 0);
                    }
            }

            }
        }
    }
    return 0;
}