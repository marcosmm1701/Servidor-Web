#ifndef CONFIG_H
#define CONFIG_H

#include  <stdio.h>
#include  <stdlib.h>
#include <string.h>
#include "parse.h"

#define CONFIG_PATH "server.conf"


typedef struct {
    char server_root[MAX_LINE];
    int max_clients;
    int listen_port;
    char server_signature[MAX_LINE];
} Config;


void load_config(const char *filename, Config *config);

#endif