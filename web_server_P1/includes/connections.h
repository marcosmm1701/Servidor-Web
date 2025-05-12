#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include <fcntl.h>

#define CONEX_QUEUE 10 // Número máximo de conexiones en espera
#define BUFFER_SIZE 1024 //lo hemos usado para almacenar la información que nos llega del cliente


// Estructura para manejar el servidor
typedef struct {
    int socket;
    struct sockaddr_in address;
} Server;

// Funciones para gestión de sockets
int create_server(int port);
int accept_connection(int server_socket);
ssize_t receive_data(int client_socket, char *buffer, size_t buffer_size);
ssize_t send_data(int client_socket, const char *data);
void close_connection(int socket);

#endif
