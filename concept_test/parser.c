#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct http_res {
    char method[16];
    char path[2048];
    char version[16];
};

int main () {
    char req_buff[] = "GET /favicon.ico HTTP/1.1\r\n"
                 "Host: localhost:8080\r\n"
                 "Connection: keep-alive\r\n"
                 "sec-ch-ua-platform: \"Linux\"\r\n"
                 "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/144.0.0.0 Safari/537.36\r\n";
    const char * const line_delimiter = "\r\n";
    char *line_start = req_buff;
    char *line_end;

    const char * const inner_delim = " ";
    char *inner_saveptr = NULL;

    struct http_res request;

    while ((line_end = strstr(line_start, line_delimiter)) != NULL) {
        *line_end = '\0';
        // processing each line
        char *token = strtok_r(line_start, inner_delim, &inner_saveptr);
        strcpy(request.method, token);
        while ()
        // end each line processing
         line_start+= strlen(line_delimiter);


    }

}