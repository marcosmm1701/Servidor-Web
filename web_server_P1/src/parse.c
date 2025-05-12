/**
 * @file parse.c
 * @brief archivo que implementa las funciones de parseo
 * Programa que implementa las funciones de parseo
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/parse.h"
#include "../includes/server.h"

/********
 * FUNCIÓN: int parse_request(char *request, Request_info *request_info)
 * ARGS_IN: char *request - petición HTTP
 *          Request_info *request_info - estructura para almacenar la información de la petición
 * DESCRIPCIÓN: Parsea la petición HTTP y almacena la información en una estructura
 * ARGS_OUT: int - 0 si termina correctamente, -1 si hay un error
 * ********/
int parse_request(char *request, Request_info *request_info)
{

    if (request == NULL || request_info == NULL)
    {
        printf("Error: request o request_info es NULL\n");
        return -1;
    }

    // Inicializamos los campos de request_info
    memset(request_info, 0, sizeof(Request_info));

    // Buffers temporales para evitar problemas de memoria
    char method[MAX_LINE], path[MAX_LINE], version[MAX_LINE];

    // Parseamos la primera línea de la petición
    if (sscanf(request, "%s %s %s", method, path, version) != 3)
    {
        printf("Error: No se ha podido parsear la primera línea de la petición\n");
        return -1;
    }

    strncpy(request_info->method, method, sizeof(request_info->method) - 1);
    strncpy(request_info->path, path, sizeof(request_info->path) - 1);
    strncpy(request_info->version, version, sizeof(request_info->version) - 1);

    // Buscamos las cabeceras de la petición
    char *header_start = strstr(request, "\r\n");

    if (header_start == NULL)
    {
        return -1;
    }

    // Avanzamos el puntero hasta el inicio de las cabeceras
    header_start += 2;

    char header_line[MAX_LINE];
    char *line_start = header_start;

    while (sscanf(line_start, "%[^\r\n]", header_line) == 1)
    {
        char *clave = strtok(header_line, ":");
        char *valor = strtok(NULL, "\n");

        if (strcmp(clave, "Connection") == 0)
        {
            strncpy(request_info->connection, valor, sizeof(request_info->connection) - 1);
        }
        else if (strcmp(clave, "Content-Length") == 0)
        {
            request_info->content_length = atoi(valor);
        }

        // Avanzamos el puntero hasta el final de la línea
        line_start = strstr(line_start, "\r\n");
        line_start += 2; // Nos saltamos el \r\n
    }

    // Buscamos el inicio del body (después de la línea en blanco \r\n\r\n)
    char *body_start = strstr(request, "\r\n\r\n");
    if (body_start != NULL)
    {
        body_start += 4; // Avanzamos después de \r\n\r\n para llegar al body
        memset(request_info->body, 0, sizeof(request_info->body)); // Resetear body a ""
        request_info->body[0] = '\0';
        strncpy(request_info->body, body_start, sizeof(request_info->body) - 1);
    }

    return 0;
}
