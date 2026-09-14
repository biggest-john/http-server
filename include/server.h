#ifndef SERVER_H
#define SERVER_H

#include <signal.h>

#define PORT "8080"
#define BACKLOG 10
#define RECV_BUFF_SIZE 1024
#define SEND_BUFF_SIZE 1024

struct HTTP_Request {
    char method[16];
    char path[2048];
    char version[16];
};

extern  int MAX_CONNECTION;
extern volatile sig_atomic_t keep_running; // main loop control variable.

//  server.c functions
int create_listening_socket();
int accept_connections();

// sig_handler.c functions
int mapper_custom_sig_handlers(int sig, void (*handler) (int));
void handle_shutdown(int sig);

// http_parser.c functions
void http_parser(char request_payload[], size_t payload_size);
#endif
