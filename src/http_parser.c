#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/server.h"

void  http_parser(char request_payload[], size_t payload_size) {
    size_t char_pointer = 0;
    size_t token_pointer = 0;
    size_t byte_count = 0;
    size_t request_params = 0;

    while(char_pointer < payload_size){
        if(request_payload[char_pointer]=='\r'){
           byte_count = char_pointer - token_pointer;
           char *version = malloc(byte_count + 1);
           if(version == NULL){
               perror("unable to allocate space for version on the heap");
               return;
           }
           memcpy(version,request_payload + token_pointer, byte_count);
           version[byte_count] = '\0';
           token_pointer = char_pointer + 1;
           break;
        }
        if(request_payload[char_pointer]==' '){
           byte_count = char_pointer - token_pointer;// reserving slot for the \0
           if(request_params == 0){
               char *method = malloc(byte_count + 1);
               if (method == NULL){
                   perror("unable to allocate space for the method on the heap");
                   return;
               }
               memcpy(method,request_payload + token_pointer, byte_count);
               method[byte_count] = '\0';
               token_pointer = char_pointer + 1;
               request_params++;
           }else if(request_params == 1){
               char *path = malloc(byte_count + 1);
               if(path == NULL){
                   perror("unable to allocate space for the path on the heap");
                   return;
               }
               memcpy(path, request_payload + token_pointer, byte_count);
               path[byte_count] = '\0';
               token_pointer = char_pointer + 1;
               request_params++;
           }
        }
        char_pointer++;
    
    }
}


