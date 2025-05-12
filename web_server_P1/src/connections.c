/**
 * @file connections.c
 * @brief archivo que implementa las funciones de conexión
 * Programa que implementa las funciones de conexión
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/connections.h"

/********
 * FUNCIÓN: int create_server(int puerto)
 * ARGS_IN: int puerto - puerto en el que escuchar
 * DESCRIPCIÓN: Crea un socket de servidor y lo pone a escuchar en un puerto
 * ARGS_OUT: int - descriptor del socket del servidor
 * ********/
int create_server(int puerto) {
    int server_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_desc == -1) {
        perror("Error al crear el socket");
        return -1;
    }

    // Configurar la opción SO_REUSEADDR para q libre el puerto inmediatamente después de cerrar el servidor
    int opt = 1;
    if (setsockopt(server_socket_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error al configurar SO_REUSEADDR");
        close(server_socket_desc);
        return -1;
    }


    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;   // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;   // para escuchar en todas las interfaces de red
    server_addr.sin_port = htons(puerto);   // puerto en orden de red

    // vinculamo el socket con la dirección y el puerto
    if (bind(server_socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al enlazar el socket");
        close(server_socket_desc);
        return -1;
    }

    // escuchamos las conexiones entrantes
    if (listen(server_socket_desc, CONEX_QUEUE) == -1) {   // CONEX_QUEUE = número máximo de conexiones pendientes
        perror("Error al escuchar en el socket");
        close(server_socket_desc);
        return -1;
    }


    // Obtener IP y puerto real del servidor
    printf("Servidor escuchando en IP: localhost, Puerto: %d\n", ntohs(server_addr.sin_port));

    return server_socket_desc;
}

/********
 * FUNCIÓN: int accept_connection(int server_socket_desc)
 * ARGS_IN: int server_socket_desc - descriptor del socket del servidor
 * DESCRIPCIÓN: Acepta una conexión entrante
 * ARGS_OUT: int - descriptor del socket del cliente
 * ********/
int accept_connection(int server_socket_desc) {
    int client_socket_desc;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    //accept es bloqueante, se queda esperando a que llegue una conexión
    client_socket_desc = accept(server_socket_desc, (struct sockaddr *)&client_addr, &client_len);

    if (client_socket_desc == -1) {
        perror("Error al aceptar la conexión");
    }

    return client_socket_desc;
}

/********
 * FUNCIÓN: ssize_t receive_data(int client_socket_desc, char *buffer, size_t buffer_size)
 * ARGS_IN: int client_socket_desc - descriptor del socket del cliente
 *          char *buffer - buffer para almacenar los datos recibidos
 *          size_t buffer_size - tamaño del buffer
 * DESCRIPCIÓN: Recibe datos de un cliente
 * ARGS_OUT: ssize_t - número de bytes recibidos
 * ********/
ssize_t receive_data(int client_socket_desc, char *buffer, size_t buffer_size) {
    return recv(client_socket_desc, buffer, buffer_size, 0);
}

/********
 * FUNCIÓN: ssize_t send_data(int client_socket_desc, const char *data)
 * ARGS_IN: int client_socket_desc - descriptor del socket del cliente
 *          const char *data - datos a enviar
 * DESCRIPCIÓN: Envía datos a un cliente
 * ARGS_OUT: ssize_t - número de bytes enviados
 * ********/
ssize_t send_data(int client_socket_desc, const char *data) {
    return send(client_socket_desc, data, strlen(data), 0);
}

/********
 * FUNCIÓN: void close_connection(int socket)
 * ARGS_IN: int socket - descriptor del socket a cerrar
 * DESCRIPCIÓN: Cierra una conexión
 * ARGS_OUT: void
 * ********/
void close_connection(int socket) {
    close(socket);
}
