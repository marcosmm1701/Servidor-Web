/**
 * @file response.c
 * @brief archivo que implementa las funciones de respuesta
 * Programa que implementa las funciones de respuesta
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/response.h"

/********
 * FUNCIÓN: void send_file(int client_socket, const char *file_path, const char *server_signature)
 * ARGS_IN: int client_socket - descriptor del socket del cliente
 *          const char *file_path - ruta del archivo a enviar
 *          const char *server_signature - firma del servidor
 * DESCRIPCIÓN: Envía un archivo al cliente
 * ARGS_OUT: void
 * ********/
void send_file(int client_socket, const char *file_path, const char *server_signature)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        // Si no se encuentra el archivo, devolvemos 404
        const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 46\r\n"
                                "\r\n"
                                "<html><body><h1>404 Not Found</h1></body></html>";
        send_data(client_socket, not_found);
        return;
    }

    // Obtener el tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Leemos el contenido
    char *content = malloc(file_size);
    if (!content)
    {
        fprintf(stderr, "Error: No se pudo reservar memoria para el archivo\n");
        fclose(file);
        return;
    }

    if (fread(content, 1, file_size, file) != file_size)
    {
        fprintf(stderr, "Error: No se pudo leer el archivo\n");
        fclose(file);
        free(content);
        return;
    }

    // Determinar el Content-Type basado en la extensión
    const char *content_type = get_mime_type(file_path);


    // Buffer para almacenar la fecha en formato HTTP
    char date_buffer[64];
    time_t now = time(NULL);
    struct tm *gm_time = gmtime(&now);

    // Formateamos la fecha según el estándar HTTP
    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gm_time);

    // Obtenemos la fecha de última modificación del archivo
    const char *last_modified = get_last_modified(file_path);

    // Enviamos encabezado HTTP
    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Date: %s\r\n"
             "Server: %s\r\n"
             "Last-Modified: %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Content-Disposition: inline\r\n"
             "\r\n",
             date_buffer, server_signature, last_modified,
             content_type, file_size);
    send_data(client_socket, header);

    // Enviamos el contenido del archivo
    size_t total_sent = 0;
    while (total_sent < file_size) {
        ssize_t sent = send(client_socket, content + total_sent, file_size - total_sent, 0);
        if (sent < 0) {
            perror("Error enviando archivo");
            free(content);
            return;
        }
        total_sent += sent;
    }


    fclose(file);
    free(content);
}

/********
 * FUNCIÓN: const char *get_mime_type(const char *file_path)
 * ARGS_IN: const char *file_path - ruta del archivo
 * DESCRIPCIÓN: Obtiene el tipo MIME de un archivo
 * ARGS_OUT: const char * - tipo MIME
 * ********/
const char *get_mime_type(const char *file_path)
{
    if (strstr(file_path, ".txt"))
        return "text/plain";
    if (strstr(file_path, ".html") || strstr(file_path, ".htm"))
        return "text/html";
    if (strstr(file_path, ".gif"))
        return "image/gif";
    if (strstr(file_path, ".jpg") || strstr(file_path, ".jpeg"))
        return "image/jpeg";
    if (strstr(file_path, ".mpeg") || strstr(file_path, ".mpg"))
        return "video/mpeg";
    if (strstr(file_path, ".doc") || strstr(file_path, ".docx"))
        return "application/msword";
    if (strstr(file_path, ".pdf"))
        return "application/pdf";


    // Extensiones extras para futuras prácticas
    if (strstr(file_path, ".png"))
        return "image/png";
    if (strstr(file_path, ".css"))
        return "text/css";
    if (strstr(file_path, ".js"))
        return "application/javascript";

    printf("Extension de archivo no encontrada");
    return "application/octet-stream";
}

/********
 * FUNCIÓN: const char *get_last_modified(const char *file_path)
 * ARGS_IN: const char *file_path - ruta del archivo
 * DESCRIPCIÓN: Obtiene la fecha de última modificación de un archivo
 * ARGS_OUT: const char * - fecha de última modificación
 * ********/
const char *get_last_modified(const char *file_path)
{
    static char last_modified[64]; // Buffer para almacenar la fecha formateada
    struct stat file_stat;

    // Obtenemos información del archivo
    if (stat(file_path, &file_stat) == -1)
    {
        perror("Error al obtener la información del archivo");
        return NULL;
    }

    // Convertimos la fecha de última modificación a tiempo GMT
    struct tm *gm_time = gmtime(&file_stat.st_mtime);

    // Formateamos la fecha según el estándar HTTP
    strftime(last_modified, sizeof(last_modified), "%a, %d %b %Y %H:%M:%S GMT", gm_time);

    return last_modified;
}
