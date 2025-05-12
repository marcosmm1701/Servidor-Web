#ifndef SERVER_H
#define SERVER_H

#include "connections.h"
#include "parse.h"
#include "scripts.h"
#include "response.h"
#include "config.h"
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>


typedef struct
{
    int client_socket_desc;
    Config config;
} ClientData;

#endif