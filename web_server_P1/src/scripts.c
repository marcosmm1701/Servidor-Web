/**
 * @file scripts.c
 * @brief archivo que implementa las funciones de scripts
 * Programa que implementa las funciones de scripts
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/scripts.h"

/********
 * FUNCIÓN: void execute_script(int client_socket, const char *file_path, const char *method, char *body)
 * ARGS_IN: int client_socket - descriptor del socket del cliente
 *          const char *file_path - ruta del script a ejecutar
 *          const char *method - método de la petición
 *          char *body - cuerpo de la petición
 * DESCRIPCIÓN: Ejecuta un script y envía la salida al cliente
 * ARGS_OUT: void
 * ********/
void execute_script(int client_socket, const char *file_path, const char *method, char *body)
{
    char command[1024];
    FILE *fp;

    // Copiamos el path para no modificar el original
    char script_path[256];
    strncpy(script_path, file_path, sizeof(script_path));
    script_path[sizeof(script_path) - 1] = '\0';

    // Extraer parámetros GET (si existen)
    char *query_params = NULL;
    query_params = strchr(script_path, '?');
    if (query_params != NULL)
    {
        *query_params = '\0'; // Separar el path de los parámetros
        query_params++;       // Mover el puntero para acceder a los parámetros
    }

    // Si hay parámetros GET, los pasamos como argumentos del script
    char params[1024] = {0};
    if (query_params)
    {
        // Separamos los parámetros por '&' y los agregamos uno por uno
        char *token = strtok(query_params, "&");
        while (token != NULL)
        {
            // Agregar el parámetro al string de parámetros
            strncat(params, token, sizeof(params) - strlen(params) - 1);
            strncat(params, " ", sizeof(params) - strlen(params) - 1);  // Espacio para separar los parámetros
            token = strtok(NULL, "&");
        }
    }


    // Construcción del comando dependiendo del método
    if (strcmp(method, "POST") == 0)
    {
        if (query_params)
        {
            // Caso mixto: Pasamos parámetros GET por argv y POST por stdin
            if (strstr(script_path, ".py") != NULL)
            {
                snprintf(command, sizeof(command), "printf \"%s\" | python3 %s %s", body ? body : "", script_path, query_params);
            }
            else
            {
                snprintf(command, sizeof(command), "printf \"%s\" | php %s %s", body ? body : "", script_path, query_params);
            }
        }
        else
        {
            // Caso POST puro: Solo enviamos body por stdin
            if (strstr(script_path, ".py") != NULL)
            {
                snprintf(command, sizeof(command), "printf \"%s\" | python3 %s", body ? body : "", script_path);
            }
            else
            {
                snprintf(command, sizeof(command), "printf \"%s\" | php %s", body ? body : "", script_path);
            }
        }
    }
    else
    {
        // Caso GET: Pasamos los parámetros GET como argumentos (argv)
        if (query_params)
        {
            if (strstr(script_path, ".py") != NULL)
            {
                int written = snprintf(command, sizeof(command), "python3 %s %s", script_path, params);
                if (written < 0 || written >= sizeof(command)) {
                    fprintf(stderr, "Error: Comando truncado\n");
                    return;
                }
            }
            else
            {
                int written = snprintf(command, sizeof(command), "php %s %s", script_path, params);
                if (written < 0 || written >= sizeof(command)) {
                    fprintf(stderr, "Error: Comando truncado\n");
                    return;
                }
            }
        }
        else
        {
            // GET sin parámetros
            if (strstr(script_path, ".py") != NULL)
            {
                snprintf(command, sizeof(command), "python3 %s", script_path);
            }
            else
            {
                snprintf(command, sizeof(command), "php %s", script_path);
            }
        }
    }

    // Ejecutar el script
    fp = popen(command, "r");
    if (!fp)
    {
        send_data(client_socket, "HTTP/1.1 500 Internal Server Error\r\n\r\nError ejecutando script");
        return;
    }

    // Leemos la salida del script y almacenarla en un buffer
    char output_buffer[4096];
    int output_len = 0;        // Longitud de la salida del script
    while (fgets(output_buffer + output_len, sizeof(output_buffer) - output_len, fp) != NULL)
    {
        output_len += strlen(output_buffer + output_len);
    }

    // Enviar los encabezados HTTP con el Content-Length
    char header[1024];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n", output_len);
    send_data(client_socket, header);

    // Enviar la salida del script (body)
    send_data(client_socket, output_buffer);

    // Cerrar el archivo y limpiar
    pclose(fp);
    memset(body, 0, strlen(body));
}
