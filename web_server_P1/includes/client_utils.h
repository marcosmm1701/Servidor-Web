#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../includes/connections.h"
#include "../includes/scripts.h"

#define RESOURCE_BUFFER 256
#define MAX_STR 256
#define NUM_CLIENTS 5
#define MAX_RESPONSE_SIZE 100000

void get_resource(int client_socket, const char *resource);
int create_client(const char *server_address, int server_port);
void run_fist_block_tests(const char *buffer);
int check_status(const char *buffer);
int check_content_type(const char *buffer);
int check_content_length(const char *buffer);
void process_images_in_buffer(char *buffer);
void check_jpeg(const char *buffer);
void process_gif_in_buffer(char *buffer);
void check_links(const char *buffer);
void check_scripts(const char *buffer, int client_socket);


#endif
