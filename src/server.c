#include  <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include "../include/server.h"


int create_listening_socket() {
    struct addrinfo hints, *res, *p;
    int status, sock_fd = -1, yes = 1; // yes for socket reuse option.

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
        p = NULL;
        return -1;
    }
    p = NULL;
    return sock_fd;
}

static int handle_client_echo(int pollfd_index, int *num_fds, struct pollfd *pfds) {
    char buff[CLIENT_MSG_BUFF_SIZE] = {0};
    ssize_t bytes_read = recv(pfds[pollfd_index].fd, buff, sizeof(buff) - 1, 0);

    if (bytes_read < 0) {
        perror("Receive Error");
    }
    else if (bytes_read == 0) {
        printf("client closed the connection!\n");
        close(pfds[pollfd_index].fd);
        pfds[pollfd_index] = pfds[*num_fds - 1]; // shrinking the coverage size and mending the hole.
        (*num_fds)--;
        return 1; // To signal the outer function that one client disconnected and we need to adjust i--.
    }
    else {
        buff[bytes_read] = '\0';
        printf("Received: %s\n", buff);

        if (send(pfds[pollfd_index].fd, buff, bytes_read, 0) == -1) {
            perror("send");
        }
    }
    return 0;
}

int accept_connections() {
    /* function to accept new connections and also client requests. A positive return value can mean there was a poll issue
     *  or an SIGINT signal was sent, in both case, we shut the server gracefully. A negative response simply mean there
     *  an issue getting the listening fd.
     */

    printf("Initializing core socket server...\n");
    struct pollfd *pfds = calloc(MAX_CONNECTIONS, sizeof(*pfds));

    const int listen_fd = create_listening_socket();
    if (listen_fd == -1) {
        fprintf(stderr, "Failed to initialize listening socket.\n");
        free(pfds);
        return -1;
    }
    printf("Server successfully listening on port %s!\n", PORT);

    struct sockaddr_storage client_storage;
    socklen_t client_size;

    //setting all fd to -1
    for (int i=0; i<MAX_CONNECTIONS; i++) {
        pfds[i].fd = -1;
    }

    pfds[0].fd = listen_fd;
    pfds[0].events = POLLIN;
    int num_fds = 1;

    while (keep_running) {
        int poll_count = poll(pfds, num_fds, -1);
        if (poll_count < 0) {
            perror("failed to setup poll correctly !");
            break;
        }

        for (int i=0;i<num_fds; i++) { // the actual scanning of the set.
            if (pfds[i].revents == 0){continue;}

            if (pfds[i].fd == listen_fd) {

                if (pfds[i].revents & POLLIN){
                    client_size = sizeof(client_storage);
                    int new_client_fd = accept(listen_fd,(struct sockaddr *) &client_storage, &client_size);

                    if (new_client_fd < 0) {
                        perror("accept error");
                    }
                    else {
                        if (num_fds < MAX_CONNECTIONS){
                            pfds[num_fds].fd = new_client_fd;
                            pfds[num_fds].events = POLLIN;
                            num_fds++;

                            printf("[Server] Connection accepted on fd %d\n", new_client_fd);
                        }
                        else {
                            printf("[Server] max connections reached: connect accept new connection.\n");
                        }
                    }
                }
            }
            else {
                if ( pfds[i].revents & POLLIN) {
                   if (handle_client_echo(i, &num_fds, pfds) == 1) {
                       i--;
                   }

                }
            }
            }
    }
    printf("\nShutting down gracefully...\n");
    for (int i=0; i<num_fds; i++) { //closing the listening socket and active client sockets
        if (pfds[i].fd != -1) {
            close(pfds[i].fd);
        }
    }
    free(pfds);
    printf("Server socket and Client socket closed...\n");
    return 0;
}