/*
 * when faced with recv() errors, I just close the file descriptor, would figure out a better way to handle this later
 */
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
    int status, sock_fd = -1;
    const int yes = 1; // yes for socket reuse option.

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
        freeaddrinfo(res);
        fprintf(stderr, "server: failed to bind socket\n");
        return -1;
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        perror("listen");
        freeaddrinfo(res);
        close(sock_fd);
        p = NULL;
        return -1;
    }
    p = NULL;
    freeaddrinfo(res);
    return sock_fd;
}

static int handle_client(int pollfd_index, int *num_fds, struct pollfd *pfds) {
    char recv_buff[RECV_BUFF_SIZE] = {};
    ssize_t bytes_recvd = recv(pfds[pollfd_index].fd, recv_buff, sizeof(recv_buff), 0); //not subtracting one from buffer size
                                                                     //because no '\0' will be there, it is just a stream of bytes

    if (bytes_recvd < 0) { // avoid merging this with connection closed because I hope to implement handling it later
        perror("Receive Error");
        close(pfds[pollfd_index].fd);
        pfds[pollfd_index] = pfds[*num_fds - 1];
        pfds[*num_fds - 1].fd = -1; pfds[*num_fds - 1].events = 0; pfds[*num_fds - 1].revents = 0;
        (*num_fds)--;
        return 1; // Signal a swap happened so the caller can adjust i--
    }
    else if (bytes_recvd == 0) {
        printf("client closed the connection!\n");
        close(pfds[pollfd_index].fd);
        pfds[pollfd_index] = pfds[*num_fds - 1]; // shrinking the coverage size and mending the hole.
        pfds[*num_fds - 1].fd = -1; pfds[*num_fds - 1].events = 0; pfds[*num_fds - 1].revents = 0;
        (*num_fds)--;
        return 1; // Signal a swap happened so the caller can adjust i--
    else {
        http_parser(recv_buff, bytes_recvd);
        /*
         * char *http_body = "<html><body>Hello World!</body></html>";
        char send_buff[SEND_BUFF_SIZE] = {0};
        const size_t send_size = snprintf(send_buff, SEND_BUFF_SIZE,  "HTTP/1.1 200 OK\r\n"
                                                                      "Content-Type: text/html\r\n"
                                                                      "Content-Length: %zu\r\n"
                                                                      "\r\n"
                                                                      "%s", strlen(http_body), http_body);
        if (send(pfds[pollfd_index].fd, send_buff, send_size, 0) == -1) {
            perror("send");
        }
        */
    }
    return 0;
}

int accept_connections() {
    printf("Initializing core socket server...\n");
    struct pollfd *pfds = calloc(MAX_CONNECTION, sizeof(*pfds));

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
    for (int i=0; i<MAX_CONNECTION; i++) {
        pfds[i].fd = -1;
    }

    pfds[0].fd = listen_fd;
    pfds[0].events = POLLIN;
    int num_fds = 1;

    while (keep_running) {
        int poll_count = poll(pfds, num_fds, -1);
        if (poll_count < 0) {// this can actually be handled more robustly. 
            perror("failed to setup poll correctly");
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
                        if (num_fds < MAX_CONNECTION){
                            pfds[num_fds].fd = new_client_fd;
                            pfds[num_fds].events = POLLIN;
                            num_fds++;

                            printf("[Server] Connection accepted on fd %d\n", new_client_fd);
                        }
                        else {
                            printf("[Server] max connections reached: Growing the pollfd set...\n");
                            const int new_limit  = MAX_CONNECTION * 2;
                            struct pollfd *temp_pfds = realloc(pfds, sizeof(*pfds) *  new_limit);
                            if (temp_pfds == NULL) {
                                perror("[REALLOC FAILED] - Server out of memory...\nCannot accept any new client at moment! ");
                                close(new_client_fd);
                                continue;
                            }
                            MAX_CONNECTION = new_limit;
                            pfds = temp_pfds;

                            for (int j = num_fds; j < MAX_CONNECTION; j++) { // initializing fds of new pollfd slots.
                                pfds[j].fd = -1;
                            }
                            temp_pfds = NULL;
                            pfds[num_fds].fd = new_client_fd;
                            pfds[num_fds].events = POLLIN;
                            num_fds++;

                            printf("[REALLOC SUCCESSFUL]");
                            printf("[Server] Connection accepted on fd %d\n", new_client_fd);
                        }
                    }
                }
            }
            else {
                if ( pfds[i].revents & POLLIN) {
                   if (handle_client(i, &num_fds, pfds) == 1) {
                       //adjusting the "i" counter so that it actually reflects the just swapped pfds
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
