/**
 * @file client.c
 * @brief archivo que implementa las funciones del cliente
 * Programa que implementa las funciones del cliente
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/client.h"

char *listen_address = "127.0.0.1";
int listen_port = 8080;

/********
 * FUNCIÓN: void handler_ctrl_c(int signal)
 * ARGS_IN: int signal - señal recibida
 * DESCRIPCIÓN: Manejador de la señal SIGINT (Ctrl+C)
 * ARGS_OUT: void
 * ********/
void handler_ctrl_c(int signal) {
    printf("\n Señal recibida. Cerrando el cliente de manera segura...\n");
    exit(0);
}

/********
 * FUNCIÓN: void process_response(char *buffer, int client_socket_desc)
 * ARGS_IN: char *buffer - buffer con la respuesta del servidor
 *          int client_socket_desc - descriptor del socket del cliente
 * DESCRIPCIÓN: Comprueba la respuesta del servidor
 * ARGS_OUT: void
 * ********/
void process_response(char *buffer, int client_socket_desc) {
    run_fist_block_tests(buffer);
    process_images_in_buffer(buffer);
    process_gif_in_buffer(buffer);
    check_links(buffer);
    check_scripts(buffer, client_socket_desc);
}

/********
 * FUNCIÓN: void run_tests(int client_socket_desc)
 * ARGS_IN: int client_socket_desc - descriptor del socket del cliente
 * DESCRIPCIÓN: Realiza las pruebas al servidor
 * ARGS_OUT: void
 * ********/
void run_tests(int client_socket_desc) {
    char response[MAX_RESPONSE_SIZE];
    ssize_t bytes_received, total_bytes_received = 0;

    memset(response, 0, sizeof(response));

    bytes_received = receive_data(client_socket_desc, response + total_bytes_received, BUFFER_SIZE);
    if (bytes_received >= 0) {
        total_bytes_received += bytes_received;
        response[total_bytes_received] = '\0'; // Asegurar terminación de string
    
        // Verificar si se ha recibido la cabecera completa
        if (strstr(response, "\r\n\r\n")) {  
    
            // Si hay Content-Length, intentar leer exactamente esa cantidad de datos
            char *content_length_header = strstr(response, "Content-Length:");
            if (content_length_header) {
                int content_length;
                sscanf(content_length_header, "Content-Length: %d", &content_length);
                
                // Calcular bytes faltantes y seguir leyendo si es necesario
                int remaining_bytes = content_length - (total_bytes_received - (strstr(response, "\r\n\r\n") - response + 4));
                while (remaining_bytes > 0 && (bytes_received = receive_data(client_socket_desc, response + total_bytes_received, BUFFER_SIZE)) > 0) {
                    total_bytes_received += bytes_received;
                    remaining_bytes -= bytes_received;
                }
            } else {
                printf("No se ha recibido Content-Length, fallo en el servidor.\n");
            }    
        }
    } else {
        printf("Error al recibir datos\n");
    }
    
    if (total_bytes_received > 0) {
        response[total_bytes_received] = '\0';
        process_response(response, client_socket_desc);
    } else {
        printf("Error al recibir datos\n");
    }
}

/********
 * FUNCIÓN: void *client_thread(void *arg)
 * ARGS_IN: void *arg - argumento del hilo
 * DESCRIPCIÓN: Función que ejecuta el hilo del cliente
 * ARGS_OUT: void
 * ********/
void *client_thread(void *arg) {
    int client_socket_desc = *((int *)arg);
    free(arg);  // Liberamos la memoria del puntero de socket

    // Realizamos la solicitud al servidor
    get_resource(client_socket_desc, "/");
    run_tests(client_socket_desc);

    close_connection(client_socket_desc);
    return NULL;
}

/********
 * FUNCIÓN: int main()
 * DESCRIPCIÓN: Función principal
 * ARGS_OUT: int - 0 si termina correctamente, -1 si hay un error
 * ********/
int main() {

    signal(SIGINT, handler_ctrl_c);

    pthread_t threads[NUM_CLIENTS];  // Array para almacenar los identificadores de los hilos

    for (int i = 0; i < NUM_CLIENTS; i++) {
        int *client_socket_desc = malloc(sizeof(int));  // Asignamos memoria para el socket de cada cliente
        *client_socket_desc = create_client(listen_address, listen_port);

        if (*client_socket_desc == -1) {
            free(client_socket_desc);
            return -1;
        }

        // Crear un hilo para cada cliente
        if (pthread_create(&threads[i], NULL, client_thread, client_socket_desc) != 0) {
            perror("Error al crear el hilo");
            close_connection(*client_socket_desc);
            free(client_socket_desc);
            return -1;
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);   
    }

    return 0;
}
