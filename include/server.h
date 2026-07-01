#ifndef SERVER_H
#define SERVER_H

#include <signal.h>

#define PORT "8080"
#define BACKLOG 10
#define MAX_CONNECTIONS 100
#define CLIENT_MSG_BUFF_SIZE 1024
/*
 * creates,bind and starts listening on a TCP socket.
 * suports both IPv4 and IPv6 via dual stack mapping
 * Returns the listening socket values or -1 (in the case of an error)
 */

extern volatile sig_atomic_t keep_running; // main loop control variable.

//  server.c functions
int create_listening_socket();
int accept_connections();

// sig_handler.c functions
int mapper_custom_sig_handlers(int sig, void (*handler) (int));
void handle_shutdown(int sig);
#endif