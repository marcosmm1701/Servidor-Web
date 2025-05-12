/**
 * @file config.c
 * @brief archivo que implementa las funciones de configuración
 * Programa que implementa las funciones de configuración
 * @version 1.0
 * @authors Marcos Muñoz e Ignacio Serena
 * @date 15/03/2025
 */

#include "../includes/config.h"

/********
 * FUNCIÓN: void load_config(const char *filename, Config *config)
 * ARGS_IN: const char *filename - nombre del archivo de configuración
 *          Config *config - estructura para almacenar la configuración
 * DESCRIPCIÓN: Carga la configuración del servidor desde un archivo
 * ARGS_OUT: void
 * ********/
void load_config(const char *filename, Config *config)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error al abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file))
    {
        // Ignoramos líneas de comentario o vacías
        if (line[0] == '#' || line[0] == '\n')
            continue;

        char key[MAX_LINE], value[MAX_LINE];

        // Extraemos clave y valor
        if (sscanf(line, "%s = %[^\n]", key, value) == 2)
        {
            if (strcmp(key, "server_root") == 0)
            {
                strcpy(config->server_root, value);
            }
            else if (strcmp(key, "max_clients") == 0)
            {
                config->max_clients = atoi(value);
            }
            else if (strcmp(key, "listen_port") == 0)
            {
                config->listen_port = atoi(value);
            }
            else if (strcmp(key, "server_signature") == 0)
            {
                strcpy(config->server_signature, value);
            }
        }
    }

    fclose(file);
}
