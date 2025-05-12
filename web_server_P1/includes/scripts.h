#ifndef SCRIPTS_H
#define SCRIPTS_H

#include "connections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void execute_script(int client_socket, const char *file_path, const char *method, char *body);

#endif