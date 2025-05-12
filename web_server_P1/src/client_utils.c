/**
 * @file client_utils.c
 * @brief Funciones auxiliares para el cliente
 * Programa que implementa funciones auxiliares para el cliente
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/client_utils.h"

/********
* FUNCIÓN: void get_resource(int client_socket, const char *resource)
* ARGS_IN: int client_socket - socket del cliente
*          const char *resource - recurso solicitado
* DESCRIPCIÓN: Envia una petición GET al servidor
* ARGS_OUT: void
********/
void get_resource(int client_socket, const char *resource) {
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: keep-alive\r\n"
        "\r\n", 
        resource);

    if (send_data(client_socket, request) == -1) {
        close_connection(client_socket);
        return;
    }
}

/********
 * FUNCIÓN: int create_client(const char *server_address, int server_port)
 * ARGS_IN: const char *server_address - dirección IP del servidor
 *          int server_port - puerto del servidor
 * DESCRIPCIÓN: Crea un socket y se conecta al servidor
 * ARGS_OUT: int - descriptor del socket del cliente
 * ********/
int create_client(const char *server_address, int server_port) {
    int sock;
    struct sockaddr_in server;

    // Crear el socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error al crear el socket");
        return -1;
    }

    // Configurar la estructura del servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    // Convertir la dirección IP de texto a formato binario
    if (inet_pton(AF_INET, server_address, &server.sin_addr) <= 0) {
        perror("Dirección inválida o no soportada");
        close(sock);
        return -1;
    }

    // Conectarse al servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error al conectar con el servidor");
        close(sock);
        return -1;
    }

    return sock;
}

/********
 * FUNCIÓN: void run_fist_block_tests(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Ejecuta las pruebas del primer bloque
 * ARGS_OUT: void
 * ********/
void run_fist_block_tests(const char *buffer) {
    printf("\nComprobando cabeceras...\n");
    if (check_status(buffer)) 
        printf("    STATUS: ✅\n");
    else
        printf("    STATUS: ❌\n");
    if (check_content_type(buffer)) 
        printf("    CONTENT-TYPE: ✅\n");
    else
        printf("    CONTENT-TYPE: ❌\n");
    if (check_content_length(buffer))
        printf("    CONTENT-LENGTH: ✅\n");
    else
        printf("    CONTENT-LENGTH: ❌\n");

    printf("\nComprobando imágenes jpeg...\n");
    check_jpeg(buffer);
}

/********
 * FUNCIÓN: void check_jpeg(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Comprueba la presencia de imágenes jpeg en el buffer
 * ARGS_OUT: void
 * ********/
void check_jpeg(const char *buffer) {
    if ((strstr(buffer, "img_big.jpeg") != NULL)) {
        printf("    Imagen big jpeg detectada: ✅\n");
    }
    else {
        printf("    Imagen big jpeg no detectada: ❌\n");
    }

    if ((strstr(buffer, "img_small.jpeg") != NULL)) {
        printf("    Imagen small jpeg detectada: ✅\n");
    }
    else {
        printf("    Imagen small jpeg no detectada: ❌\n");
    }
}

/********
 * FUNCIÓN: int check_status(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Comprueba si la respuesta del servidor es 200 OK
 * ARGS_OUT: int - 1 si es 200 OK, 0 si no
 * ********/
int check_status(const char *buffer) {
    if (strstr(buffer, "HTTP/1.1 200 OK") != NULL) {
        return 1;
    }
    return 0;
}

/********
 * FUNCIÓN: int check_content_type(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Comprueba si la respuesta del servidor tiene Content-Type: text/html
 * ARGS_OUT: int - 1 si es text/html, 0 si no
 * ********/
int check_content_type(const char *buffer) {
    if (strstr(buffer, "Content-Type: text/html") != NULL) {
        return 1;
    }
    return 0;
}

/********
 * FUNCIÓN: int check_content_length(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Comprueba si la respuesta del servidor tiene Content-Length
 * ARGS_OUT: int - 1 si tiene Content-Length, 0 si no
 * ********/
int check_content_length(const char *buffer) {
    if (strstr(buffer, "Content-Length: ") != NULL) {
        return 1;
    }
    return 0;
}

/********
 * FUNCIÓN: void process_images_in_buffer(char *buffer)
 * ARGS_IN: char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Procesa las imágenes jpg en el buffer
 * ARGS_OUT: void
 * ********/
void process_images_in_buffer(char *buffer) {
    char img_tag[50];  // Aumenté el tamaño del buffer para manejar números más grandes
    char *current_pos = buffer;
    int img_count = 1;

    printf("\nComprobando imágenes jpg...\n");

    while (1) {
        // Crear el nombre de la imagen con el prefijo 'media/'
        snprintf(img_tag, sizeof(img_tag), "media/img%d.jpg", img_count);

        // Buscar la imagen actual en el buffer desde la posición actual
        char *found = strstr(current_pos, img_tag);

        // Si encontramos la imagen
        if (found != NULL) {
            printf("    Imagen jpg %d detectada: ✅\n", img_count);

            // Mover el puntero al final de la imagen encontrada para seguir buscando desde allí
            current_pos = found + strlen(img_tag);

            // Incrementamos el contador para la siguiente imagen
            (img_count)++;
        } else {
            // Si no encontramos más imágenes, terminamos
            break;
        }
    }
}

/********
 * FUNCIÓN: void process_gif_in_buffer(char *buffer)
 * ARGS_IN: char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Procesa las imágenes gif en el buffer
 * ARGS_OUT: void
 * ********/
void process_gif_in_buffer(char *buffer) {
    printf("\nComprobando imágenes gif...\n");
    if(strstr(buffer, "media/animacion.gif") != NULL) {
        printf("    Imagen gif detectada: ✅\n");
    } else {
        printf("    Imagen gif no detectada: ❌\n");
    }
}

/********
 * FUNCIÓN: void check_links(const char *buffer)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 * DESCRIPCIÓN: Comprueba los enlaces en el buffer
 * ARGS_OUT: void
 * ********/
void check_links(const char *buffer) {
    printf("\nComprobando enlaces...\n");
    if (strstr(buffer, "kjhdsfkjhdk.html") != NULL) {
        printf("    Enlace a recurso no existente detectado: ✅\n");
    } else {
        printf("    Enlace a recurso no existente no detectado: ❌\n");
    }

    if (strstr(buffer, "IMPORTANTE.txt") != NULL) {
        printf("    Enlace a IMPORTANTE.txt detectado: ✅\n");
    } else {
        printf("    Enlace a IMPORTANTE.txt no detectado: ❌\n");
    }

    if (strstr(buffer, "media/video.mpeg") != NULL) {
        printf("    Enlace a video.mpeg detectado: ✅\n");
    } else {
        printf("    Enlace a video.mpeg no detectado: ❌\n");
    }

    if (strstr(buffer, "media/texto.txt") != NULL) {
        printf("    Enlace a texto.txt detectado: ✅\n");
    } else {
        printf("    Enlace a texto.txt no detectado: ❌\n");
    }
}

/********
 * FUNCIÓN: void check_scripts(const char *buffer, int client_socket)
 * ARGS_IN: const char *buffer - buffer con la respuesta del servidor
 *          int client_socket - socket del cliente
 * DESCRIPCIÓN: Comprueba los scripts en el buffer
 * ARGS_OUT: void
 * ********/
void check_scripts(const char *buffer, int client_socket) {
    printf("\nComprobando scripts...\n");

    // Verificar script Python: test.py
    if (strstr(buffer, "action=\"scripts/test.py\"") != NULL) {
        printf("    Formulario para test.py (GET/POST) detectado: ✅\n");
    } else {
        printf("    Formulario para test.py no detectado: ❌\n");
    }

    // Verificar script PHP: test.php
    if (strstr(buffer, "action=\"scripts/test.php\"") != NULL) {
        printf("    Formulario para test.php (GET/POST) detectado: ✅\n");
    } else {
        printf("    Formulario para test.php no detectado: ❌\n");
    }

    // Verificar script Python: hola.py (GET)
    if (strstr(buffer, "action=\"scripts/hola.py\"") != NULL) {
        printf("    Formulario para hola.py (GET) detectado: ✅\n");
    } else {
        printf("    Formulario para hola.py no detectado: ❌\n");
    }

    // Verificar script Python: fecha.py (GET)
    if (strstr(buffer, "action=\"scripts/fecha.py\"") != NULL) {
        printf("    Formulario para fecha.py (GET) detectado: ✅\n");
    } else {
        printf("    Formulario para fecha.py no detectado: ❌\n");
    }

    // Verificar script Python: convertir_temp.py (POST)
    if (strstr(buffer, "action=\"scripts/convertir_temp.py\"") != NULL) {
        printf("    Formulario para convertir_temp.py (POST) detectado: ✅\n");
    } else {
        printf("    Formulario para convertir_temp.py no detectado: ❌\n");
    }

    // Verificar script Python: calculadora.py (GET)
    if (strstr(buffer, "action=\"scripts/calculadora.py\"") != NULL) {
        printf("    Formulario para calculadora.py (GET) detectado: ✅\n");
    } else {
        printf("    Formulario para calculadora.py no detectado: ❌\n");
    }
}