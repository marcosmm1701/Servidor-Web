#ifndef PARSE_H
#define PARSE_H

#define MAX_LINE 1024

typedef struct {
    char method[16];
    char path[MAX_LINE];
    char version[16];
    char connection[64];
    int content_length;
    char body[MAX_LINE];
} Request_info;

int parse_request(char *request, Request_info *request_info);

#endif