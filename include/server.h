#ifndef SERVER_H
#define SERVER_H

#define PORT "8080"
#define BACKLOG 10
/*
 * creates,bind and starts listening on a TCP socket.
 * suports both IPv4 and IPv6 via dual stack mapping
 * Returns the listening socket values or -1 (in the case of an error)
 */
int create_listening_socket();
void handle_client_echo(int client_fd);

#endif