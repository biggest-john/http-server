#include  <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "../include/server.h"


int create_listening_socket() {
    struct addrinfo hints, *res, *p;
    int status, sock_fd, yes = 1; // yes for socket reuse option.

    memset(&hints, 0, sizeof(hints)); // pre-setting the struct to empty
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddressinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    for (p=res; p!=NULL;p=p->ai_next) {
        // create socket and binding to the socket to any of the addrinfo structs in the linked list res
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock_fd == -1) {
            continue;
        }
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            // catching error when trying to setsockopt to immediately reuseable.
            perror("setsocketopt");
            close(sock_fd);
            freeaddrinfo(res);
            res = NULL;
            return -1;
        }

        if(bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            continue;
        }

        break; // break successfully on first complete combination of socket and binding.
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        return -1;
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        perror("listen");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

void handle_client_echo(const int client_fd) {
    char buff[1024] = {0};
    int bytes_read = recv(client_fd, buff, sizeof(buff) - 1, 0);

    if (bytes_read < 0) {
        perror("recv");
    } else if ( bytes_read == 0) {
        printf("client closed the connection!\n");
    }else {
        buff[bytes_read] = '\0';
        printf("Received: %s\n", buff);

        if (send(client_fd, buff, sizeof(buff), 0) == -1) {
            perror("send");
        }
    }
    close(client_fd);
}