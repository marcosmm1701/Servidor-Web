#ifndef RESPONSE_H
#define RESPONSE_H

#include <netinet/in.h>
#include "connections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Para stat
#include <time.h>     // Para strftime y time

void send_file(int client_socket, const char *file_path, const char *server_signature);
const char *get_mime_type(const char *file_path);
const char *get_last_modified(const char *file_path);

#endif