#ifndef CLIENT_H
#define CLIENT_H

#include "../includes/connections.h"
#include "../includes/client_utils.h"
#include <signal.h>
#include <assert.h>
#include <pthread.h>

void handler_ctrl_c(int signal);
void close_all_sockets(int *client_socket_desc, int num_clients);
void process_response(char *buffer, int client_socket_desc);
void run_tests(int client_socket_desc);

#endif