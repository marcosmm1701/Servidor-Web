/**
 * @file server.c
 * @brief archivo que implementa las funciones del servidor
 * Programa que implementa las funciones del servidor
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */
#include "../includes/server.h"

// Descriptor del socket del servidor
int server_socket_desc;

// Número de clientes activos
int active_clients = 0;

// Mutex para controlar el acceso a la variable active_clients
sem_t semaforo;

/********
 * FUNCIÓN: void handler_ctrl_c(int senal)
 * ARGS_IN: int senal - señal recibida
 * DESCRIPCIÓN: Manejador de la señal SIGINT (Ctrl+C)
 * ARGS_OUT: void
 * ********/
void handler_ctrl_c(int senal)
{
    printf("\n Señal recibida. Cerrando el servidor de manera segura...\n");

    close_connection(server_socket_desc);
    server_socket_desc = -1;
    sem_destroy(&semaforo);
    exit(0); // Cierra el programa de manera segura
}



/********
 * FUNCIÓN: void *handle_client(void *arg)
 * ARGS_IN: void *arg - argumento del hilo
 * DESCRIPCIÓN: Función que ejecuta el hilo del cliente
 * ARGS_OUT: void
 * ********/
void *handle_client(void *arg)
{
    ClientData client_data = *(ClientData *)arg;
    int client_socket_desc = client_data.client_socket_desc;
    Config config = client_data.config;

    free(arg);
    char buffer[BUFFER_SIZE];
    int keep_alive = 1;
    int timeout = 60;


    while (keep_alive && (server_socket_desc != -1) && timeout > 0)
    {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = receive_data(client_socket_desc, buffer, BUFFER_SIZE);

        if (bytes_received == 0)
        {
            sleep(1);
            timeout--;
            continue;
        }
        timeout = 60;

        if (bytes_received == -1)
        {
            perror("Error en recv()");
            break;
        }

        Request_info request_info = {0};
        parse_request(buffer, &request_info);

        if ((strcmp(request_info.version, "HTTP/1.0") == 0) | (strcmp(request_info.connection, "close") == 0))
        {
            keep_alive = 0;
        }


        if (strcmp(request_info.method, "GET") == 0)
        {
            char file_path[MAX_LINE];

            // Verifica el tamaño de la ruta combinada
            size_t root_len = strlen(config.server_root);
            size_t path_len = strlen(request_info.path);
            size_t total_len = root_len + path_len;

            if (total_len >= sizeof(file_path))
            {
                fprintf(stderr, "Error: Ruta demasiado larga. Tamaño necesario: %zu\n", total_len);
                const char *response = "HTTP/1.1 500 Internal Server Error.\r\n"
                                       "Content-Length: 0\r\n"
                                       "\r\n";
                send_data(client_socket_desc, response);
            }
            else
            {
                // Si la ruta es /, se debe devolvemos el index.html
                if (strcmp(request_info.path, "/") == 0)
                {
                    int written = snprintf(file_path, sizeof(file_path), "%s/index.html", config.server_root);
                    if (written < 0 || written >= sizeof(file_path))
                    {
                        fprintf(stderr, "Error: Ruta del archivo truncada\n");
                        break;
                    }
                }
                else
                {
                    int written = snprintf(file_path, sizeof(file_path), "%s%s", config.server_root, request_info.path);
                    if (written < 0 || written >= sizeof(file_path))
                    {
                        fprintf(stderr, "Error: Ruta del archivo truncada\n");
                        break;
                    }
                }

                // Verificamos si es un script a ejecutar
                if (strstr(file_path, ".py") || strstr(file_path, ".php"))
                {
                    // Llamamos a la función para ejecutar scripts con el body de la petición
                    execute_script(client_socket_desc, file_path, request_info.method, request_info.body);
                }
                else
                {
                    // Enviamos el archivo solicitado
                    send_file(client_socket_desc, file_path, config.server_signature);
                }
            }
        }
        else if (strcmp(request_info.method, "POST") == 0)
        {
            char file_path[MAX_LINE];
            int written = snprintf(file_path, sizeof(file_path), "%s%s", config.server_root, request_info.path);
            if (written < 0 || written >= sizeof(file_path))
            {
                fprintf(stderr, "Error: Ruta del archivo truncada\n");
                break;
            }

            // Verificamos si es un script a ejecutar
            if (strstr(request_info.path, ".py") || strstr(request_info.path, ".php"))
            {
                // Llamamos a la función para ejecutar scripts con el body de la petición
                execute_script(client_socket_desc, file_path, request_info.method, request_info.body);
            }
            else
            {
                // Si no es un script, devolvemos un error 405.Ya que no se puede hacer un POST a un archivo
                const char *response = "HTTP/1.1 405 Method Not Allowed\r\n"
                                       "Content-Length: 0\r\n"
                                       "\r\n";
                send_data(client_socket_desc, response);
            }
        }
        else if (strcmp(request_info.method, "OPTIONS") == 0)
        {
            // Construimos la ruta del recurso solicitado
            char file_path[MAX_LINE];
            int written = snprintf(file_path, sizeof(file_path), "%s%s", config.server_root, request_info.path);
            if (written < 0 || written >= sizeof(file_path))
            {
                fprintf(stderr, "Error: Comando truncado\n");
                break;
            }

            // Verificamos si el archivo existe
            if (access(file_path, F_OK) != -1)
            {
                const char *allow_header;
                // Si es un script, permitir GET y POST
                if (strstr(file_path, ".py") || strstr(file_path, ".php"))
                {
                    allow_header = "Allow: GET, POST, OPTIONS\r\n";
                }
                else
                {
                    // Para otros archivos estáticos (HTML, imágenes, CSS, JS...), solo permitir GET y OPTIONS
                    allow_header = "Allow: GET, OPTIONS\r\n";
                }

                // Enviar respuesta con los métodos permitidos
                char response[256];
                snprintf(response, sizeof(response),
                         "HTTP/1.1 204 No Content\r\n"
                         "%s"
                         "Content-Length: 0\r\n"
                         "\r\n",
                         allow_header);

                send_data(client_socket_desc, response);
            }
            else
            {
                // El recurso no existe, enviamos 404 Not Found
                const char *response = "HTTP/1.1 404 Not Found\r\n"
                                       "Content-Length: 0\r\n"
                                       "\r\n";
                send_data(client_socket_desc, response);
            }
        }
        else
        {
            // Si el método no es GET, POST o OPTIONS, devolvemos un error 405
            const char *response = "HTTP/1.1 405 Method Not Allowed\r\n"
                                   "Content-Length: 0\r\n"
                                   "\r\n";
            send_data(client_socket_desc, response);
        }
    }

    sem_wait(&semaforo);
    active_clients--;
    sem_post(&semaforo);

    close_connection(client_socket_desc);
    pthread_exit(NULL);
}


/********
 * FUNCIÓN: int main()
 * DESCRIPCIÓN: Función principal
 * ARGS_OUT: int - 0 si termina correctamente, -1 si hay un error
 * ********/
int main()
{
    Config config;
    load_config(CONFIG_PATH, &config);
    int listen_port = config.listen_port;
    pthread_t threads[config.max_clients];
    int thread_count = 0;

    // Registramos el manejador de la señal SIGINT (Ctrl+C)
    signal(SIGINT, handler_ctrl_c);

    server_socket_desc = create_server(listen_port);
    if (server_socket_desc == -1)
    {
        perror("Error al crear el servidor");
        return -1;
    }

    sem_init(&semaforo, 0, 1);

    while (1)
    {
        sem_wait(&semaforo);
        // Verificamos si hemos alcanzado el límite de clientes simultáneos
        if (active_clients >= config.max_clients)
        {
            printf("Número máximo de clientes alcanzado. Esperando...\n");

            // Esperamos un momento antes de intentar aceptar una nueva conexión
            sleep(1);
            continue;
        }

        int client_socket_desc;
        client_socket_desc = accept_connection(server_socket_desc);
        if (client_socket_desc == -1)
        {
            close_connection(server_socket_desc);
            return -1;
        }

        active_clients++;
        sem_post(&semaforo);

        // Crear una estructura para pasar los datos del cliente al hilo
        ClientData *client_data = (ClientData *)malloc(sizeof(ClientData));
        if (client_data == NULL)
        {
            perror("Error al asignar memoria para los datos del cliente");
            close_connection(client_socket_desc);
            continue; // O salir de la función si no se puede continuar
        }
        client_data->client_socket_desc = client_socket_desc;
        client_data->config = config;

        // Crear un hilo para manejar al cliente

        if (pthread_create(&threads[thread_count], NULL, handle_client, (void *)client_data) != 0)
        {
            perror("Error al crear el hilo");
            close_connection(client_socket_desc);
            free(client_data); // Liberar la memoria asignada
        }
        else
        {
            thread_count++;
        }
    }
    return 0;
}
